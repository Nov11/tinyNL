//
// Created by c6s on 17-2-23.
//

#include "Acceptor.h"
#include "AddressUtility.h"
#include <tinyNL/net/Channel.h>
#include <tinyNL/net/EventLoop.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <tinyNL/base/Log.h>
#include <sstream>

namespace tinyNL {
    namespace net {
        Acceptor::Acceptor(EventLoop *loop, int port)
                : loop_(loop),
                  socket_(Socket::getNonBlockingTcpSocket()),
                  channel_(socket_.fd(), loop_) {
            //1.bind
            socket_.setReuseAddr(true);
            socket_.bind(port);

//            //4.enable channel
//            cptr->enableRead();
        }

        Acceptor::~Acceptor() {
            channel_.disableChannel();
        }

        void Acceptor::start() {
            loop_->assertInLoopThread();

            //2.listen
            socket_.listen();
            //3.setup channel callback
            channel_.setReadCallBack([this]() { this->channelReadCallBack(); });
            channel_.enableRead();
        }

        void Acceptor::channelReadCallBack() {
            loop_->assertInLoopThread();
            struct sockaddr_in addr;
            int incommingSocketFd = socket_.accept4(&addr);
            if (incommingSocketFd == -1) {
                return;
            }
            //new connection and set it up with eventloop
            std::stringstream tmp;
            tmp<< "get new connection :";
            tmp<<AddressUtility::toString(addr)<<" port: "<<ntohs(addr.sin_port)<<std::endl;
            base::LOG<<tmp.str();
            //for test
            //close(incommingSocketFd);
            if (userCallBack_) {
                userCallBack_(incommingSocketFd, addr);
            }
        }
    }
}