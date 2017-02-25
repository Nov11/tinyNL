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
#include <tinyNL/net/Acceptor.h>

namespace tinyNL{
    namespace net{
        class EventLoop;
        class Acceptor;
        class TcpConnection;
        class Buffer;
        //this is supposed to be a stack obj and lives as long as eventloop
        //so srv lives longer than tcp connections
        //
        class TcpServer :Noncopyable, public std::enable_shared_from_this<TcpServer>{
        public:
            typedef std::function<void (std::shared_ptr<TcpConnection>)> CallBack;
            explicit TcpServer(EventLoop* loop, int port);

            //for client use
            void setOnConnectionCallBack(const CallBack& cb){onConnection_ = cb;}
            void setOnMessageCallBack(const CallBack& cb){onMessage_ = cb;}
            void setOnPeerCloseCallBack(const CallBack& cb){onPeerClose_ = cb;}
            ~TcpServer()= default;
            void start();
            void demolish();//do not expect to restart server after calling this
            void addWorkEventLoop(EventLoop* loop){workerLoop_.push_back(loop);}
        private:
            EventLoop* loop_;
            std::vector<EventLoop*> workerLoop_;
            //release resource using destor
            std::unique_ptr<Acceptor> accptr;
            std::set<std::shared_ptr<TcpConnection>> connectionList;
            void newConnection(int fd, sockaddr_in&);
            void removeConnectionFromOwner(const std::shared_ptr<TcpConnection>&);
            void removeConnectionFromOwnerInLoopThread(const std::shared_ptr<TcpConnection>&);
            CallBack onPeerClose_;
            CallBack onConnection_;
            CallBack onMessage_;
            void stopInLoopThread();
            //should set a limit to number of active connections
            int cnt = 0;
            EventLoop* pickWorkLoop();
        };
    }
}

#endif //TINYNL_TCPSERVER_H
