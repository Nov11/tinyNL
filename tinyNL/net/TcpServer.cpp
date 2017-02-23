//
// Created by c6s on 17-2-23.
//

#include "TcpServer.h"
#include <tinyNL/net/Acceptor.h>
#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpConnection.h>

namespace tinyNL{
    namespace net{

        TcpServer::TcpServer(EventLoop *loop, int port)
        :loop_(loop),
        accptr(new Acceptor(loop_, port)){

        }

        void TcpServer::start() {
            auto f = std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2);
            std::function<void(int, sockaddr_in&)> func = f;
            accptr->setUserCallBack(func);
            accptr->start();
        }

        void TcpServer::newConnection(int fd, sockaddr_in& peer) {
            loop_->assertInLoopThread();
            //1.new tcpconnection
            std::shared_ptr<TcpConnection> ptr(new TcpConnection(loop_, fd, peer));
            //2.save it up
            connectionList.insert(ptr);
            //3.set call back
            ptr->setOnMsgCallBack(onMessage_);
            ptr->setOnConnectionCallBack(onConnection_);
            //4.set connection to work
            ptr->start();
        }
    }
}