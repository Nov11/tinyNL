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
            EventLoop* worker = pickWorkLoop();
            std::shared_ptr<TcpConnection> ptr(new TcpConnection(worker, fd, peer));
            //2.save it up
            connectionList.insert(ptr);
            //3.set call back
            ptr->setOnMsgCallBack(onMessage_);
            ptr->setOnConnectionCallBack(onConnection_);
            ptr->setOnPeerCloseCallBack(onPeerClose_);

            std::weak_ptr<TcpServer> weak_ptr = shared_from_this();
            ptr->setSelfRemoveCallBack([weak_ptr](const std::shared_ptr<TcpConnection>& param){
                std::shared_ptr<TcpServer> sptr = weak_ptr.lock();
                if(sptr){
                    sptr->removeConnectionFromOwner(param);
                }else{
                    //server is destructed, no need to remove connction
                }
            });
            //this is called when acceptor received a new connection
            if(onConnection_){
                onConnection_(ptr);
            }
            //4.set connection to work
            ptr->start();
        }


        void TcpServer::removeConnectionFromOwnerInLoopThread(const std::shared_ptr<TcpConnection> &ptr) {
            loop_->assertInLoopThread();
            connectionList.erase(ptr);
        }

        void TcpServer::removeConnectionFromOwner(const std::shared_ptr<TcpConnection>& ptr) {
            auto tmp = std::bind(&TcpServer::removeConnectionFromOwnerInLoopThread, this, ptr);
            loop_->runInLoopThread(tmp);
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
                iter->closeConnection();
            }
        }

        EventLoop *TcpServer::pickWorkLoop() {
            if(workerLoop_.empty()){return loop_;}
            cnt ++;
            auto idx = cnt % workerLoop_.size();
            return workerLoop_[idx];
        }
    }
}