//
// Created by c6s on 17-2-23.
//

#include "TcpConnection.h"
#include <tinyNL/net/Channel.h>
#include <unistd.h>
#include <tinyNL/base/Log.h>

namespace tinyNL {
    namespace net {

        TcpConnection::TcpConnection(EventLoop *loop, int fd, sockaddr_in &addr)
                : loop_(loop),
                  socket_(fd),
                  cptr(new Channel(fd, loop_)),
                  peer_(addr) {
            cptr->setReadCallBack([this]() { channelRead(); });
            cptr->setWriteCallBack([this]() { channelWrite(); });

        }

        TcpConnection::~TcpConnection() {
            cptr->disableChannel();
        }

        void TcpConnection::start() {
            cptr->enableReadAndWrite();
        }

        void TcpConnection::channelRead() {
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
                    cptr->disableRead();
                    break;
                } else {
                    //ret == -1
                    if (errno == EINTR) {
                        continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        //it's ok. data in sock buffer is drained
                        break;
                    } else {
                        base::LOG.logErrorAndExit();
                    }
                }
            }
            readBuf.append(buf, input);
            //2.pass this buf to user callback
            onMsgcb_(readBuf, *this);
        }

        void TcpConnection::channelWrite() {
//            //1.if there is still some msg left in the buffer, write them
//            writeBuf.writeToSocket(socket_);
//            //2.if wrtite buf is cleared, disable channel write.
//            //  or there will be busy loop
//            if (writeBuf.empty()) {
//                cptr->disableWrite();
//            }

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
                        //peer closed;
                    }else{
                        base::LOG.logErrorAndExit();
                    }
                }
            }
            writeBuf.erase(output);
            if(writeBuf.readableSize() > 0){
                //when socket fd is writable, write remaining data

            }else{
                cptr->disableWrite();
            }
        }

        void TcpConnection::closeConnection() {
            closing_ = true;

        }


    }
}