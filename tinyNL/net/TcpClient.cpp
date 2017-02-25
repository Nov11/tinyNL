//
// Created by c6s on 17-2-25.
//

#include <tinyNL/base/Log.h>
#include "TcpClient.h"
#include <tinyNL/net/TcpConnection.h>


namespace tinyNL {
    namespace net {

        TcpClient::TcpClient(EventLoop *loop, sockaddr_in &peer) :
                loop_(loop),
                peer_(peer),
                connector_(new TcpConnector(loop_, peer)) {
        }

        void TcpClient::start() {
            connector_->setConnectionEstablishedCB([this](int fd){connectionEstablished(fd);});
            connector_->setConnectFailureCallBack([this](){connectionFailed();});
            connector_->tryConnect();
        }

        void TcpClient::connectionEstablished(int socketfd) {
//            if(scb_){scb_(socketfd);}//a little weird
            std::shared_ptr<TcpConnection> con(new TcpConnection(loop_, socketfd, peer_));
            if(onConnected_){
                onConnected_(con);
            }
            con->setOnPeerCloseCallBack(onPeerClose_);
            con->setOnMsgCallBack(onMsg_);
            connection_ = con;
            connection_->start();
        }

        void TcpClient::connectionFailed() {
            base::LOG<<"tcpclient connect failed\n";
            //may be set a status or something?
            if (fcb_) {
                fcb_();
            }
        }

        void TcpClient::demolish() {
            if(!connector_->active()){return;}
            if(!connector_->conneted()){connector_.reset();return;}//free that obj
            connection_->closeConnection();
        }
    }
}