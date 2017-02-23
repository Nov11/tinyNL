//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_ACCEPTOR_H
#define TINYNL_ACCEPTOR_H

#include <tinyNL/dir/Noncopyable.h>
#include <functional>
#include <memory>
#include <netinet/in.h>
#include "Socket.h"


namespace tinyNL{
    namespace net{
        class EventLoop;
        class Channel;
        class Acceptor : Noncopyable{
        public:
            typedef std::function<void(int, sockaddr_in)> AcceptorUserCallBack;
            explicit Acceptor(EventLoop *loop, int port, const AcceptorUserCallBack & userCallBack);
            ~Acceptor();
        private:
            EventLoop* loop_;
            Socket socket_;
            std::shared_ptr<Channel> cptr;
            void channelReadCallBack();
            AcceptorUserCallBack userCallBack_;
        };
    }
}



#endif //TINYNL_ACCEPTOR_H
