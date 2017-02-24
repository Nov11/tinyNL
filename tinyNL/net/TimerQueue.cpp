//
// Created by c6s on 17-2-21.
//

#include "TimerQueue.h"
#include <sys/timerfd.h>
#include <sys/time.h>
#include <tinyNL/base/Log.h>
#include <tinyNL/net/Channel.h>
#include <tinyNL/net/Timer.h>
#include <tinyNL/net/EventLoop.h>
#include <cstring>
#include <tinyNL/base/TimeUtilies.h>

namespace tinyNL {
    namespace net {
        TimerQueue::TimerQueue(EventLoop *loop)
                : loop_(loop),
                  timerfd_(::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC)),
                  timerQueue(cmp),
                  channel_(timerfd_, loop_) {
            if (timerfd_ == -1) { tinyNL::base::LOG.logErrorAndExit(); }
            //set call back
            //deal with life time range later
            //timerqueue lives as long as its owner eventloop , which owns multiplexer,
            //so I think it is ok to publish 'this' pointer here
            rcb = [this]() { readCallBack(); };
            //std::bind(TimerQueue::channelReadCallBack, this, std::placeholders::_1);
            channel_.setReadCallBack(rcb);
            channel_.enableRead();
        }


        TimerQueue::~TimerQueue() {
            channel_.disableChannel();
            close(timerfd_);
        }

        void TimerQueue::readCallBack() {
            loop_->assertInLoopThread();
            //read timerfd, or there will be busy loop
            //at least 8-byte
            uint64_t timeMsg;
            ssize_t ret = read(timerfd_, &timeMsg, sizeof(timeMsg));
            if (ret == -1 && errno != EINTR) {
                base::LOG.logErrorAndExit();
            }
            //maybe do this in pending functions in future
            expireLogic();
        }

        void TimerQueue::expireLogic() {
            //get expiredTimer tasks
            auto expiredTimers = getExpiredTimersAndRemoveThemFromTimerQueue();
            auto pendingTasks = insertRepeatableTimerAndGenePendingTasks(expiredTimers);
            //queue them
            loop_->addPendingTasks(pendingTasks);
            //find the nearest timer that will expire and set it up
            if (timerQueue.empty()) {
                //disable timerfd
                setNextTimeAlarm(false, 0);
            } else {
                std::shared_ptr<Timer> sptr = *timerQueue.begin();
                setNextTimeAlarm(true, sptr->startTime());
            }
        }


        bool TimerQueue::cmp(const std::shared_ptr<Timer> &left, const std::shared_ptr<Timer> &right) {
            long r = left->startTime() - right->startTime();
            if (r < 0) {
                return true;
            } else if (r > 0) {
                return false;
            } else {
                auto lptr = left.get();
                auto rptr = right.get();
                return lptr < rptr;
            }
        }

        //this should be run in the eventlopp's owner thread
        std::vector<std::shared_ptr<Timer>> TimerQueue::getExpiredTimersAndRemoveThemFromTimerQueue() {
            loop_->assertInLoopThread();

            //little weird to use shared ptr when cmp
            auto iter = timerQueue.lower_bound(std::shared_ptr<Timer>(
                    new Timer(nullptr, base::TimeUtilies::millionSecondsSinceEposh(), 0, 0)));
            std::vector<std::shared_ptr<Timer>> ret(timerQueue.begin(), iter);
            timerQueue.erase(timerQueue.begin(), iter);
            //this is supposed to be rvo
            return ret;
        }

        std::vector<std::function<void()>>
        TimerQueue::insertRepeatableTimerAndGenePendingTasks(std::vector<std::shared_ptr<Timer>> &expiredTimerList) {
            long now = base::TimeUtilies::millionSecondsSinceEposh();
            std::vector<std::function<void()>> tasks;
            for (auto &ptr : expiredTimerList) {
                //get callback from timer
                tasks.push_back(ptr->callBack());
                //if timer is repeatable, update its time and insert them into timerqueue
                if (ptr->repeatable() && ptr->interval() > 0) {
                    //update start time to now + interval
                    //in case server running slow, use start + interval will generate infinite tasks
                    ptr->setStartTime(ptr->interval() + now);
                    timerQueue.insert(ptr);
                }
            }
            return tasks;
        }

        void TimerQueue::setNextTimeAlarm(bool on, long millionSeconds) {
            itimerspec it;
            ::memset(&it, 0, sizeof(it));
            if (on) {
                it.it_value.tv_sec = millionSeconds / 1000;
                it.it_value.tv_nsec = (millionSeconds % 1000) * 1000 * 1000;
            }
            int ret = timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &it, nullptr);
            if (ret != 0) {
                base::LOG.logErrorAndExit();
            }
        }

        void TimerQueue::addTimerInEventLoop(const std::shared_ptr<Timer> &timer) {
            loop_->assertInLoopThread();
            timerQueue.insert(timer);
            if (timerQueue.size() == 1) {
                //what if timer is already expired, will sys wake the proc up
                setNextTimeAlarm(true, timer->startTime());
            }
        }

        void TimerQueue::addTimer(const std::shared_ptr<Timer> &timer) {
            //this is ok to chain these to one line. but clion always show red underline. why?
            auto tmp = std::bind(&TimerQueue::addTimerInEventLoop, this, timer);
            std::function<void()> function = tmp;
            loop_->runInLoopThread(function);
        }

        void TimerQueue::delTimer(const std::shared_ptr<Timer> &timer) {
            auto tmp = std::bind(&TimerQueue::delTimerInEventLoop, this, timer);
            loop_->runInLoopThread(tmp);
        }

        void TimerQueue::delTimerInEventLoop(const std::shared_ptr<Timer> &delThisTimer) {
            loop_->assertInLoopThread();
            timerQueue.erase(delThisTimer);
        }
    }
}

