//
// Created by c6s on 17-2-23.
//

#include "TcpConnection.h"
#include <tinyNL/net/Channel.h>
#include <unistd.h>
#include <tinyNL/base/Log.h>
#include <tinyNL/net/EventLoop.h>

namespace tinyNL {
    namespace net {
        const int TcpConnection::WRITEBUFUPPERLIMIT = 1024;
        TcpConnection::TcpConnection(EventLoop *loop, int fd, sockaddr_in &addr)
                : loop_(loop),
                  socket_(fd),
                  channel_(fd, loop_),
                  peer_(addr) {
            channel_.setReadCallBack([this]() { channelRead(); });
            channel_.setWriteCallBack([this]() { channelWrite(); });

        }

        TcpConnection::~TcpConnection() {
            channel_.disableChannel();
            base::LOG<<"tcp connection destor\n";
        }

        void TcpConnection::start() {
            auto tmp = std::bind(&TcpConnection::startInLoop, this);
            loop_->runInLoopThread(tmp);
        }

        void TcpConnection::startInLoop() {
            channel_.enableReadAndWrite();
        }

        void TcpConnection::channelRead() {
            loop_->assertInLoopThread();
            //force closing or received EPIPE
            if(closing_){return;}
            //1.read from fd
//            readBuf.readFromSocket(socket_);
            //epoll uses level trigger, so here it's ok to not drain sock buffer
            char buf[256] = {};
            size_t input = 0;
            size_t len = sizeof(buf);
            char *ptr = buf;
            while (input < len) {
                ssize_t ret = ::read(socket_.fd(), ptr, len - input);
                if (ret > 0) {
                    input += ret;
                    ptr += ret;
                } else if (ret == 0) {
                    //peer close
                    //first I think I should drain writebuff and then close this connection.
                    //but if peer closes socket write channel, it will never send back app level ack.
                    //even if writebuf is drained,
                    //host app level user code can't tell if the message in writebuff is received by peer.
                    //so why bother drain writebuf. just go on to connection shutdown.

                    //shutdown here, cause might have read some msg in ealry while loop.
                    //shutdown connection after read buffer is processed by app level user code.
                    base::LOG<<"read 0 peer close\n";

                    if(input == 0){
                        closeConnectionInLoopThread();
                        return;
                    }else{
                        channel_.disableChannel();
                        closing_ = true;
                        auto tmp = std::bind(&TcpConnection::closeConnectionInLoopThread, shared_from_this());
                        loop_->addPendingTask(tmp);
                    }
                    break;
                } else {
                    //ret == -1
                    if (errno == EINTR) {
                        continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        //it's ok. data in sock buffer is drained
                        break;
                    } else {
//                        base::LOG.logErrorAndExit();
                        //shutdown program is a little over doing
                        //just close this connection
                        closeConnectionInLoopThread();
                        return;
                    }
                }
            }
            auto retApp = readBuf.append(buf, input);
            if(!retApp){
                base::LOG << "read buf overflow. shutting connection down";
                closeConnection();
            }
//            //2.pass this buf to user callback,run in pending queue;
//            auto tmp = std::bind(onMsgcb_, shared_from_this());
//            loop_->runInLoopThread(tmp);
            if(onMsgcb_){onMsgcb_(shared_from_this());}

        }

        void TcpConnection::channelWrite() {
//            //1.if there is still some msg left in the buffer, write them
//            writeBuf.writeToSocket(socket_);
//            //2.if wrtite buf is cleared, disable channel write.
//            //  or there will be busy loop
//            if (writeBuf.empty()) {
//                cptr->disableWrite();
//            }
            loop_->assertInLoopThread();
            //in channel dispatching, read first and then write.
            //if read process detects that peer closed, the connection should shutdown.
            //read process already set one closing funtor to pending functor queue
            //there is no point to write remaining data in write buff as explained in channelread.
            //even if program insists on writing remaining data in writebuf,
            //peer socket close can only be detected no earlier than second write.
            //and it possible that this process set up second closeconnection functor to pending queue.
            //this will lead to second invocation of user's on peer close callback.
            //it's ok to call tcpserver's remove callback, since it removes this connection only,
            // no matter this connection has already been removed or not, no disaster will be caused.
            //so here is a closing_ check, that if connection is closing, no matter set by
            // closeconnection or read or write, just ignore remaining data and no writing will be done any more.
            if(closing_){return;}
            size_t output = 0;
            char* ptr = writeBuf.readPtr();
            size_t len = writeBuf.readableSize();
            while(output < len){
                ssize_t ret = ::write(socket_.fd(), ptr, len - output);
                if(ret > 0){
                    ptr += ret;
                    output += ret;
                }else if(ret == 0){
                    //well
                    base::LOG<<"write return 0";
                }else{
                    if(errno == EINTR){
                        continue;
                    }else if(errno == EAGAIN || errno == EWOULDBLOCK){
                        break;
                    }else if(errno == EPIPE){
                        base::LOG.logError();
                        //peer closed socket; stop writing; demolish reading; do user callback; close connection;
                        closeConnectionInLoopThread();
                        return;
                    }else{
                        base::LOG.logError();
                        closeConnectionInLoopThread();
                        return;
                    }
                }
            }
            writeBuf.erase(output);
            if(writeBuf.readableSize() > 0){
                //if break from EAGAIN | EWOULDBLOCK, just return to channel event dispatcher
                //next time when socket fd is writable, write remaining data
                //<del>if break from EPIPE, connection close procedure will be done during pending function processing</del>
            }else{
                if(onWriteComplete_){
                    //avoid stack overflow if onWrite calls send which eagerly calls write again
                    //for now, it's ok to run writeComplete in next dopendingtask
                    auto tmp = std::bind(onWriteComplete_, shared_from_this());
                    //or the pending task will not be invoked until io multiplexer timed out.
                    loop_->queueInLoopAndWakeLoopThread(tmp);
                }else{
                    channel_.disableWrite();
                }
            }
        }

        void TcpConnection::closeConnectionInLoopThread() {
            base::LOG<<"tcp connection closeConnectionInLoopThread\n";
            //1.call user level close call back
            //2.remove itself from tcpserver
            loop_->assertInLoopThread();
            channel_.disableChannel();
            closing_ = true;
            std::shared_ptr<TcpConnection> con = shared_from_this();
            if(removeFromSrv_){
                removeFromSrv_(con);
            }
            if(onPeerClose_){
                onPeerClose_(con);
            }
            //con des
        }

        void TcpConnection::shutdownWrite() {
            loop_->assertInLoopThread();
            socket_.shutdownWR();
            channel_.disableWrite();
        }

        void TcpConnection::closeConnection() {
            auto tmp = std::bind(&TcpConnection::closeConnectionInLoopThread, shared_from_this());
            loop_->runInLoopThread(tmp);
        }

        void TcpConnection::send(const std::string &str) {
            auto tmp = std::bind(&TcpConnection::sendInLoop, shared_from_this(), str);
            loop_->runInLoopThread(tmp);
        }

        void TcpConnection::sendInLoop(const std::string &str) {
            loop_->assertInLoopThread();
            if(closing_){return;}
            if(writeBuf.readableSize() > WRITEBUFUPPERLIMIT){
                closeConnection();
            }
            writeBuf.append(str.data(), str.size());
            channel_.enableWrite();
            //eager send not sure about its impact
            channelWrite();
        }

        std::string TcpConnection::read() {
            loop_->assertInLoopThread();
            char *ptr = readBuf.readPtr();
            size_t len = readBuf.readableSize();
            std::string ret(ptr, len);
            readBuf.erase(len);
            return ret;
        }

        void TcpConnection::closeConnectionOnWriteBuffDrained() {
            auto tmp = std::bind(&TcpConnection::closeConnectionOnWriteBuffDrainedInLoop, shared_from_this());
            loop_->runInLoopThread(tmp);
        }

        void TcpConnection::closeConnectionOnWriteBuffDrainedInLoop() {
            loop_->assertInLoopThread();
            if(writeBuf.readableSize() == 0){
                closeConnection();
            }else{
                auto tmp = std::bind(&TcpConnection::closeConnectionOnWriteBuffDrainedInLoop, shared_from_this());
                loop_->addPendingTask(tmp);
            }
        }
    }
}