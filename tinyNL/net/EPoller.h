//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_EPOLLER_H
#define TINYNL_EPOLLER_H


#include <tinyNL/net/Multiplexer.h>
#include <vector>
#include <sys/epoll.h>

namespace tinyNL{
    namespace net{
        class Channel;
        class EPoller : public Multiplexer{
        public:
            EPoller();
            ~EPoller();
            int wait(::epoll_event* evlist, int maxevents, int timeoutMillionSecond);
            //not used yet
            int modify(int op, int fd, ::epoll_event* event);
            void multiplexerPolling(int timeoutMillionSecond, ChannelList &readyChannelList) override;

            void multiplexerUpdate(Channel* channel) override ;
        private:
            typedef std::vector<struct epoll_event> EpollCtrlList;
            void fillActiveChannelList(int number, ChannelList &channelList, EpollCtrlList& epollCtrlList);

        private:
            int epollfd_;
        };
    }
}



#endif //TINYNL_EPOLLER_H
