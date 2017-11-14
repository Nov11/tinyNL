//
// Created by c6s on 17-2-25.
//

#ifndef TINYNL_TCPCONNECTOR_H
#define TINYNL_TCPCONNECTOR_H

#include <tinyNL/dir/Noncopyable.h>
#include <memory>
#include <tinyNL/base/Cond.h>
#include <netinet/in.h>
#include <functional>

namespace tinyNL {
    namespace net {
        class EventLoop;
        class Channel;
        class TcpConnector : Noncopyable , public std::enable_shared_from_this<TcpConnector> {
        public:
            typedef std::function<void(int)> ConnectionEstablishedCallBack;
            typedef std::function<void()> ConnectFailureCallBack;
            TcpConnector(EventLoop* loop, sockaddr_in&);
            ~TcpConnector();
            void tryConnect();
            void setConnectionEstablishedCB(const ConnectionEstablishedCallBack & cb){cecb_= cb;}
            void setConnectFailureCallBack(const ConnectFailureCallBack& cb){cfcb_=cb;}
            bool active() const {return active_;}
            bool conneted() const {return connected_;};
        private:
            EventLoop* loop_;
            sockaddr_in peer_;
            int socket_;//pass this out after connection established, do not close it in destor
            std::unique_ptr<Channel> cptr;//rather allocate new one than clear existing one
            void writeCallBack();
            bool connected_;
            bool active_;
            ConnectionEstablishedCallBack cecb_;
            ConnectFailureCallBack cfcb_;
            int tried;
            const int retry = 4;
            static int retryInval[5]; //what interval should I use if failed n time? retryInval[n]
            void tryConnectingAgain();
            void alreadyConnected();
            static void tryConnectTimerTask(std::weak_ptr<TcpConnector> weak_ptr);
        };

    }
}


#endif //TINYNL_TCPCONNECTOR_H
