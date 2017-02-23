//
// Created by c6s on 17-2-23.
//

#include "Acceptor.h"
#include <tinyNL/net/Channel.h>
#include <tinyNL/net/EventLoop.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

namespace tinyNL {
    namespace net {
        Acceptor::Acceptor(EventLoop *loop, int port)
                : loop_(loop),
                  socket_(Socket::getNonBlockingTcpSocket()),
                  cptr(new Channel(socket_.fd(), loop_)) {
            //1.bind
            socket_.setReuseAddr(true);
            socket_.bind(port);

//            //4.enable channel
//            cptr->enableRead();
        }

        Acceptor::~Acceptor() {
            cptr->disableChannel();
        }

        void Acceptor::start() {
            loop_->assertInLoopThread();

            //2.listen
            socket_.listen();
            //3.setup channel callback
            cptr->setReadCallBack([this]() { this->channelReadCallBack(); });
            cptr->enableRead();
        }

        void Acceptor::channelReadCallBack() {
            loop_->assertInLoopThread();
            struct sockaddr_in addr;
            int incommingSocketFd = socket_.accept4(&addr);
            if (incommingSocketFd == -1) {
                return;
            }
            //new connection and set it up with eventloop
            std::cout << "get new connection fd:" << std::endl;
            //for test
            //close(incommingSocketFd);
            if (userCallBack_) {
                std::cout<< "acceptor run user Call Back" <<std::endl;
                userCallBack_(incommingSocketFd, addr);
            }
        }
    }
}