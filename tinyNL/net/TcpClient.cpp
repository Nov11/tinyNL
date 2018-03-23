//
// Created by c6s on 17-2-25.
//

#include <tinyNL/base/Log.h>
#include "TcpClient.h"
#include <tinyNL/net/EventLoop.h>
#include <cassert>

namespace tinyNL {
    namespace net {

        TcpClient::TcpClient(EventLoop *loop, sockaddr_in &peer) :
                loop_(loop),
                peer_(peer),
                connector_(new TcpConnector(loop_, peer)) {
        }

        void TcpClient::start() {
            loop_->assertInLoopThread();
            connector_->setConnectionEstablishedCB([this](int fd){connectionEstablished(fd);});
            connector_->setConnectFailureCallBack([this](){connectionFailed();});
            connector_->tryConnect();
        }

        void TcpClient::connectionEstablished(int socketfd) {
            loop_->assertInLoopThread();
//            if(scb_){scb_(socketfd);}//a little weird
            std::shared_ptr<TcpConnection> con(new TcpConnection(loop_, socketfd, peer_));
            if(onConnected_){
                onConnected_(con);
            }
            con->setOnPeerCloseCallBack(onPeerClose_);
            con->setOnMsgCallBack(onMsg_);
            std::weak_ptr<TcpClient> weak_ptr = shared_from_this();
            auto tmpf = std::bind(&TcpClient::removeConnectionFromTcpClientCallBack, weak_ptr, std::placeholders::_1);
            con->setSelfRemoveCallBack(tmpf);
            connection_ = con;
            connection_->start();
        }

        void TcpClient::connectionFailed() {
            loop_->assertInLoopThread();
            base::LOG<<"tcpclient connect failed\n";
            //may be set a status or something?
            if (fcb_) {
                fcb_();
            }
        }

        void TcpClient::demolish() {
            loop_->assertInLoopThread();
            if(!connector_->active()){return;}
            if(!connector_->connected()){connector_.reset();return;}//free that obj
            if(connection_){
                connection_->closeConnection();
            }
        }

        void TcpClient::removeConnectionFromTcpClient(const std::shared_ptr<TcpConnection> &con) {
            loop_->assertInLoopThread();
            assert(con == connection_);
            connection_.reset();
        }

        void TcpClient::removeConnectionFromTcpClientCallBack(const std::weak_ptr<TcpClient> &wptr,
                                                              const std::shared_ptr<TcpConnection> &con) {
            std::shared_ptr<TcpClient> sptr = wptr.lock();
            if(sptr){
                sptr->removeConnectionFromTcpClient(con);
            }else{
                //tcpclient has expired already
            }
        }
    }
}