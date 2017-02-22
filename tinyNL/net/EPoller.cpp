//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/EPoller.h>
#include <tinyNL/base/Log.h>
#include <tinyNL/net/Channel.h>
#include <cstring>
#include <iostream>

namespace tinyNL {
    namespace net {

        EPoller::EPoller() {
            epollfd_ = ::epoll_create1(EPOLL_CLOEXEC);
            if (epollfd_ == -1) {
                //call this before log, cause log will call 'write' which  clears errno set by epollcreate
                std::string errorMsg(strerror(errno));
                tinyNL::base::LOG << "error creating epoll fd : " + errorMsg;
                //there is nothing we can do
                _exit(1);
            }
        }

        EPoller::~EPoller() {
            close(epollfd_);
        }

        int EPoller::wait(::epoll_event *evlist, int maxevents, int timeoutMillionSecond) {
            return epoll_wait(epollfd_, evlist, maxevents, timeoutMillionSecond);
        }

        int EPoller::modify(int op, int fd, struct epoll_event *event) {
            return epoll_ctl(epollfd_, op, fd, event);
        }

        void EPoller::multiplexerPolling(int timeoutMillionSecond, Multiplexer::ChannelList &readyChannelList) {
            //later cache this in epoller obj
            const int maxevents = 100;
            EpollCtrlList evlist(maxevents);
            int ret = wait(&*evlist.begin(), maxevents, timeoutMillionSecond);
            if (ret < 0) {
                if(errno == EINTR){
                    tinyNL::base::LOG << "interrupted during epoll_wait";
                }else{
                    tinyNL::base::LOG << (std::string("error epoll_wait:") + strerror(errno));
                    _exit(1);
                }
            } else if (ret == 0) {
                tinyNL::base::LOG << "epoll wait time out. \n";
            } else {
                fillActiveChannelList(ret, readyChannelList, evlist);
            }
        }

        void EPoller::fillActiveChannelList(int number, Multiplexer::ChannelList &readyChannelList,
                                            EpollCtrlList &epollCtrlList) {
            ChannelList newChannelList;
            for (int i = 0; i < number; i++) {
                struct epoll_event ee = epollCtrlList[i];
                Channel *ch = static_cast<Channel *>(ee.data.ptr);
                ch->setRevent(ee.events);
                newChannelList.push_back(ch);
            }
            std::swap(readyChannelList, newChannelList);
        }

        void EPoller::multiplexerUpdate(Channel *channel) {
            //del
            if (channel->isDisabled()) {
                int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->fd(), nullptr);
                if (ret == -1) {
                    tinyNL::base::LOG << strerror(errno) << " action : epoll ctrl del";
                }
                return;
            }

            struct epoll_event ee;
            ee.events = channel->events();
            ee.data.ptr = channel;
            //mod
            if (channel->isAdded()) {
                int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, channel->fd(), &ee);
                if (ret == -1) {
                    tinyNL::base::LOG << strerror(errno) << " action : epoll ctrl mod";
                }
                return;
            }
            //add
            int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->fd(), &ee);
            if (ret == -1) {
                tinyNL::base::LOG << strerror(errno) << " action : epoll ctrl add";
            }else{
                channel->setAddedIntoMultiplexer(true);
            }
        }
    }
}
