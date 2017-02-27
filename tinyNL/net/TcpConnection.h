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
#include <tinyNL/net/Channel.h>

namespace tinyNL{
    namespace net{
        class Channel;
        class EventLoop;
        class Buffer;
        //in the view of server side, if a peer shuts down, the connection shuts down.
        class TcpConnection : Noncopyable, public std::enable_shared_from_this<TcpConnection>{
        public:
            typedef std::function<void (const std::shared_ptr<TcpConnection>&)> CallBack;
            typedef std::function<void(const std::shared_ptr<TcpConnection>&)> SelfRemoveFromSrv;
            TcpConnection(EventLoop *loop, int fd, sockaddr_in &addr);
            ~TcpConnection();
            void start();
            void shutdownWrite();
            void closeConnectionInLoopThread();
            void closeConnection();
            void setOnMsgCallBack(CallBack& onMsg){onMsgcb_ = onMsg;}
            void setOnConnectionCallBack(const CallBack& onConcb){onConnectioncb_=onConcb;}
            void setOnPeerCloseCallBack(const CallBack& cb){onPeerClose_ = cb;}
            void setSelfRemoveCallBack(const SelfRemoveFromSrv&cb){removeFromSrv_=cb;}
            void setOnWriteComplete(const CallBack& cb){onWriteComplete_ = cb;}
            Buffer readBuf;//readFromSocket from fd
            sockaddr_in peerAddr() const {return peer_;};
            void send(const std::string& str);//if writebuf reach a certain limit, shutdown connection
            std::string read();
        private:
            EventLoop* loop_;
            Socket socket_;
            Channel channel_;
            sockaddr_in peer_;
            Buffer writeBuf;//writeToSocket to fd
            void channelRead();
            void channelWrite();
            CallBack onMsgcb_;
            CallBack onConnectioncb_;//not use yet
            CallBack onPeerClose_;
            CallBack onWriteComplete_;//seldom used
            SelfRemoveFromSrv removeFromSrv_;
            bool closing_ = false;
            void sendInLoop(const std::string& str);
            void startInLoop();
            const static int WRITEBUFUPPERLIMIT;
        };
    }
}



#endif //TINYNL_TCPCONNECTION_H
