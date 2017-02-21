//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_TIMERQUEUE_H
#define TINYNL_TIMERQUEUE_H

#include <tinyNL/dir/Noncopyable.h>

#include <memory>
#include <vector>
#include <set>

namespace tinyNL{
    namespace net{
        class Channel;
        class Timer;
        class EventLoop;
        class TimerQueue : Noncopyable{
        public:
            TimerQueue(EventLoop* eventLoop);
            ~TimerQueue();
            void addTimer(std::shared_ptr<Timer>&);

        private:
            int timerfd_;
            EventLoop* loop_;
            std::set<std::shared_ptr<Timer>> timerQueue;
            std::shared_ptr<Channel> channelPtr_;
            void removeExpiredTimer();
            void readCallBack();
            std::function<void()> rcb;
        };
    }
}



#endif //TINYNL_TIMERQUEUE_H
