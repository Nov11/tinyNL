//
// Created by c6s on 17-2-25.
//

#ifndef TINYNL_TCPCLIENT_H
#define TINYNL_TCPCLIENT_H

#include "TcpConnector.h"
#include "TcpConnection.h"

namespace tinyNL{
    namespace net{
        class EventLoop;
        //suppose client lives longer than its tcpconnector
        class TcpClient :Noncopyable {
        public:
            //connector
//            typedef std::function<void(int)> Succ;
            typedef std::function<void()> Fail;

            //connection
            typedef std::function<void (std::shared_ptr<TcpConnection>)> ConCallBack;
            TcpClient(EventLoop* loop, sockaddr_in& peer);
            ~TcpClient()=default;
            void start();
            void demolish();
            void setConnectionFailCallBack(const Fail& f){ fcb_ = f; }
//            void setConnectionEstablishCB(const Succ& s){scb_ = s;}
            void setOnMsgCB(const ConCallBack& cb){ onMsg_ = cb; }
            void setOnPeerClose(const ConCallBack& cb){ onPeerClose_ = cb; }
            void setOnConnected(const ConCallBack& cb){onConnected_ = cb;}
        private:
            EventLoop* loop_;
            sockaddr_in peer_;
            std::shared_ptr<TcpConnector> connector_;
            std::shared_ptr<TcpConnection> connection_;
            void connectionEstablished(int);
            void connectionFailed();
            Fail fcb_;
//            Succ scb_;
            ConCallBack onMsg_;
            ConCallBack onPeerClose_;
            ConCallBack onConnected_;
        };
    }
}
#endif //TINYNL_TCPCLIENT_H
