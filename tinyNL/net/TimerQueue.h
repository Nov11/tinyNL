//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_TIMERQUEUE_H
#define TINYNL_TIMERQUEUE_H

#include <tinyNL/dir/Noncopyable.h>

#include <memory>
#include <vector>
#include <set>
#include "Channel.h"

namespace tinyNL {
    namespace net {
        class Channel;

        class Timer;

        class EventLoop;

        class TimerQueue : Noncopyable {
        public:
            //one timerqueue should be associate with only one eventloop
            //it's weird that one timer queue deals with multi eventloop
            explicit TimerQueue(EventLoop *eventLoop);

            ~TimerQueue();

            void addTimer(const std::shared_ptr<Timer> &);

            void delTimer(const std::shared_ptr<Timer> &);

        private:
            //order by start time, if start time are the same, order by ptr of time obj
            static bool cmp(const std::shared_ptr<Timer> &left, const std::shared_ptr<Timer> &right);

            //avoid annoying grammar checkout from clion
            typedef decltype(cmp) *cmptype;
            EventLoop *loop_;
            int timerfd_;
            //timerqueue owns every timer. only modified in owner thread of eventloop
            //if timer obj is huge, pass smart pointer is better than copy timer obj.
            //well, in retrospec unique_ptr's overhead is better, but assignment code is weired to write.
            //for now, there is no much difference between using smart pointer and copying obj.
            //for example, stack obj timer (copied several times as param)-> copied to funtor
            // -> (if not in loop thread)funtor copied to pending task queue -> functor run , timer passed as param
            // -> timer copied into timerQueue
            std::set<std::shared_ptr<Timer>, cmptype> timerQueue;
            Channel channel_;
            std::function<void()> rcb;

            void setNextTimeAlarm(bool on, long millionSeconds);

            void addTimerInEventLoop(const std::shared_ptr<Timer> &);

            void delTimerInEventLoop(const std::shared_ptr<Timer> &);

            void readCallBack();

            void expireLogic();

            std::vector<std::function<void()>>
            insertRepeatableTimerAndGenePendingTasks(std::vector<std::shared_ptr<Timer>> &expiredTimerList);

            std::vector<std::shared_ptr<Timer>> getExpiredTimersAndRemoveThemFromTimerQueue();
        };
    }
}


#endif //TINYNL_TIMERQUEUE_H
