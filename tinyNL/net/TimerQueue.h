//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_TIMERQUEUE_H
#define TINYNL_TIMERQUEUE_H

#include <tinyNL/dir/Noncopyable.h>

#include <memory>
#include <vector>
#include <set>

namespace tinyNL {
    namespace net {
        class Channel;

        class Timer;

        class EventLoop;

        class TimerQueue : Noncopyable {
        public:
            //one timerqueue should be associate with only one eventloop
            //it's weird that one timer queue deals with multi eventloop
            TimerQueue(EventLoop *eventLoop);

            ~TimerQueue();

            void addTimer(const std::shared_ptr<Timer> &);

            void delTimer(const std::shared_ptr<Timer> &);

        private:
            //order by start time, if start time are the same, order by ptr of time obj
            static bool cmp(const std::shared_ptr<Timer> &left, const std::shared_ptr<Timer> &right);

            //avoid annoying grammar checkout from clion
            typedef decltype(cmp) *cmptype;
            int timerfd_;
            EventLoop *loop_;
            //timerqueue owns every timer. only modified in owner thread of eventloop
            std::set<std::shared_ptr<Timer>, cmptype> timerQueue;
            std::shared_ptr<Channel> channelPtr_;
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
