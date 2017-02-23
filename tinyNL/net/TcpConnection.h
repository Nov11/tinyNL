//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_TCPCONNECTION_H
#define TINYNL_TCPCONNECTION_H

#include <functional>
#include <memory>
#include "Socket.h"
#include "Buffer.h"
#include "TcpServer.h"

namespace tinyNL{
    namespace net{
        class Channel;
        class EventLoop;
        class Buffer;
        class TcpConnection : Noncopyable{
        public:
            typedef std::function<void (Buffer&, TcpConnection&)> CallBack;
            explicit TcpConnection(EventLoop *loop, int fd, sockaddr_in &addr);
            ~TcpConnection();
            void start();
            void shutdownWrite();
            void closeConnection();
            void setOnMsgCallBack(CallBack& onMsg){onMsgcb_ = onMsg;}
            void setOnConnectionCallBack(CallBack& onConcb){onConnectioncb_=onConcb;}
        private:
            EventLoop* loop_;
            Socket socket_;
            std::shared_ptr<Channel> cptr;
            sockaddr_in peer_;
            Buffer readBuf;//readFromSocket from fd
            Buffer writeBuf;//writeToSocket to fd
            void channelRead();
            void channelWrite();
            CallBack onMsgcb_;
            CallBack onConnectioncb_;
            bool closing_ = false;
        };
    }
}



#endif //TINYNL_TCPCONNECTION_H
