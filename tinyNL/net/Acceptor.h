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
#include "Channel.h"


namespace tinyNL{
    namespace net{
        class EventLoop;
        class Channel;

        //1.build acceptor
        //2.set uesr call back which deals with incomming fd fd
        //3.start the acceptor which register itself to io multiplexer
        class Acceptor : Noncopyable{
        public:
            typedef std::function<void(int, sockaddr_in&)> AcceptorUserCallBack;
            Acceptor(EventLoop *loop, int port);
            void setUserCallBack(const AcceptorUserCallBack & userCallBack){userCallBack_=userCallBack;}
            void start();
            ~Acceptor();
        private:
            EventLoop* loop_;
            Socket socket_;
            Channel channel_;
            void channelReadCallBack();
            AcceptorUserCallBack userCallBack_;
        };
    }
}



#endif //TINYNL_ACCEPTOR_H
