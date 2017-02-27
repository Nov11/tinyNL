//
// Created by c6s on 17-2-25.
//

#include "TcpConnector.h"
#include <tinyNL/net/Channel.h>
#include <tinyNL/net/EventLoop.h>
#include <unistd.h>
#include <tinyNL/base/Log.h>
#include <assert.h>
#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>


namespace tinyNL {
    namespace net {
        int TcpConnector::retryInval[] = {0,1,2,4,8};
        TcpConnector::TcpConnector(EventLoop *loop, sockaddr_in &peer)
                : loop_(loop),
                  peer_(peer),
                  socket_(Socket::getNonBlockingTcpSocket()),
                  cptr(new Channel(socket_, loop_)),
                  connected_(false),
                  active_(false),
                  tried(0) {}

        void TcpConnector::writeCallBack() {
            loop_->assertInLoopThread();
            cptr->disableWrite();
            int op = 0;
            socklen_t len = sizeof(op);
            int ret = ::getsockopt(socket_, SOL_SOCKET, SO_ERROR, &op, &len);
            if (ret == -1) {
                base::LOG.logErrorAndExit();
            }
            if (op == 0) {
//                sockaddr_in local;
//                socklen_t tlen = sizeof(local);
//                int tret = ::getsockname(socket_, reinterpret_cast<sockaddr*>(&local), &tlen);
//                if(tret == -1){
//                    base::LOG.logErrorAndExit();
//                }

                alreadyConnected();
            } else {
                base::LOG.logError(op);
                tryConnectingAgain();
            }
        }

        void TcpConnector::tryConnect() {
            loop_->assertInLoopThread();
            assert(connected_ == false);
            active_ = true;
            int ret = ::connect(socket_, reinterpret_cast<sockaddr *>(&peer_), sizeof(peer_));
            if (ret == 0) {
                //rarely happens
                alreadyConnected();
            } else {
                //ret == -1
                if (errno == EINPROGRESS) {
                    //this imples tcpconnector must at least live until the 4th retry and call ed cfcb_

                    auto tmp = std::bind(&TcpConnector::writeCallBack, this);
                    cptr->setWriteCallBack(tmp);
                    cptr->enableWrite();
                } else{
                    //deal with error
                    if (errno == EINTR) {
                        //should not encounter this with nonblocking socket
                        base::LOG<<"connect with EINTR";
                    } else {
                        base::LOG.logError();
                    }
                    //delay and retry
                    tryConnectingAgain();
                }
            }
        }

        TcpConnector::~TcpConnector() {
            cptr->disableChannel();
        }

        void TcpConnector::tryConnectingAgain() {
            loop_->assertInLoopThread();
            cptr->disableChannel();
            close(socket_);
            tried++;
            if(tried <= retry) {
                socket_ = Socket::getNonBlockingTcpSocket();
                cptr.reset(new Channel(socket_, loop_));
                std::weak_ptr<TcpConnector> weak_ptr(shared_from_this());
                auto tmp = std::bind(&TcpConnector::tryConnectTimerTask, weak_ptr);
                loop_->addTimerSinceNow(tmp, retryInval[tried] * 1000, 0, 0);
            }else{
                base::LOG<<"failed to establish connection, exit \n";
                if(cfcb_){cfcb_();}
                active_ = false;
            }
        }

        void TcpConnector::alreadyConnected() {
            connected_ = true;
            //this will remove fd from multiplexer
            cptr->disableChannel();
            //do not call cb before disablechannel
            if (cecb_) {
                cecb_(socket_);
            }
        }

        void TcpConnector::tryConnectTimerTask(std::weak_ptr<TcpConnector> weak_ptr) {
            std::shared_ptr<TcpConnector> sptr = weak_ptr.lock();
            if(sptr == nullptr){
                return;
            }
            sptr->tryConnect();
        }
    }
}