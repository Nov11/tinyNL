//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_TCPSERVER_H
#define TINYNL_TCPSERVER_H


#include <tinyNL/dir/Noncopyable.h>
#include <memory>
#include <netinet/in.h>
#include <vector>
#include <set>

namespace tinyNL{
    namespace net{
        class EventLoop;
        class Acceptor;
        class TcpConnection;
        class Buffer;
        class TcpServer :Noncopyable{
        public:
            typedef std::function<void (Buffer&, TcpConnection&)> CallBack;
            explicit TcpServer(EventLoop* loop, int port);

            //for client use
            void setOnConnectionCallBack(CallBack& cb){onConnection_ = cb;}
            void setOnMessageCallBack(CallBack& cb){onMessage_ = cb;}
            ~TcpServer()= default;
            void start();
        private:
            EventLoop* loop_;
            std::shared_ptr<Acceptor> accptr;
            std::set<std::shared_ptr<TcpConnection>> connectionList;
            void newConnection(int fd, sockaddr_in&);
            CallBack onConnection_;
            CallBack onMessage_;
            //should set a limit to number of active connections
        };
    }
}

#endif //TINYNL_TCPSERVER_H
