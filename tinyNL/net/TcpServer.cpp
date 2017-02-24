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
            loop_->assertInLoopThread();
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
            ptr->setOnPeerCloseCallBack(onPeerClose_);

            ptr->setSelfRemoveCallBack([this](const std::shared_ptr<TcpConnection>& param){ removeConnectionFromOwner(param); });
            //this is called when acceptor received a new connection
            if(onConnection_){
                onConnection_(ptr);
            }
            //4.set connection to work
            ptr->start();
        }

        void TcpServer::removeConnectionFromOwner(const std::shared_ptr<TcpConnection>& ptr) {
            loop_->assertInLoopThread();
            connectionList.erase(ptr);
        }

        void TcpServer::demolish() {
            loop_->runInLoopThread([this](){ stopInLoopThread();});
        }

        void TcpServer::stopInLoopThread() {
            loop_->assertInLoopThread();
            //delete acceptor
            accptr.reset();
            //close related connections
            decltype(connectionList) localList;
            swap(localList, connectionList);
            //tcpconection will invalid iterator of connectionList,
            //so here uses a local one, whose tcp connection will run destor after this function returns.
            for (auto &iter :localList) {
                iter->closeConnectionInLoopThread();
            }
            std::cout<<"destor"<<std::endl;
        }
    }
}