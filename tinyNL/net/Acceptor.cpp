//
// Created by c6s on 17-2-23.
//

#include "Acceptor.h"
#include <tinyNL/net/Channel.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

namespace tinyNL {
    namespace net {
        void Acceptor::channelReadCallBack() {
            struct sockaddr_in addr;
            int incommingSocketFd = socket_.accept4(&addr);
            if (incommingSocketFd == -1) {
                return;
            }
            //new connection and set it up with eventloop
            std::cout << "get new connection fd:" << std::endl;
            //for test
            //close(incommingSocketFd);
            userCallBack_(incommingSocketFd, addr);
        }

        Acceptor::Acceptor(EventLoop *loop, int port, const AcceptorUserCallBack &userCallBack)
                : loop_(loop),
                  socket_(Socket::getNonBlockingTcpSocket()),
                  cptr(new Channel(socket_.socket(), loop_)),
                  userCallBack_(userCallBack) {
            //1.bind
            socket_.setReuseAddr(true);
            socket_.bind(port);
            //2.listen
            socket_.listen();
            //3.setup channel callback
            cptr->setReadCallBack([this]() { this->channelReadCallBack(); });
            //4.enable channel
            cptr->enableRead();
        }

        Acceptor::~Acceptor() {
            cptr->disableChannel();
        }
    }
}