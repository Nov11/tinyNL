//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_EVENTLOOP_H
#define TINYNL_EVENTLOOP_H

#include <tinyNL/dir/Noncopyable.h>
#include <tinyNL/net/CurrentThread.h>
#include <tinyNL/base/Mutex.h>

#include <vector>
#include <iostream>
#include <memory>

#include <utmpx.h>
#include <atomic>

namespace tinyNL {
    namespace net {
        class Channel;

        class Multiplexer;

        class TimerQueue;

        class Timer;

        class EventLoop : Noncopyable {
        public:
            EventLoop();

            ~EventLoop();

            void loop();

            void stop();

            bool isInLoopThread() const {
                return threadId_ == tinyNL::net::CurrentThread::tid();
            }

            void assertInLoopThread() {
                if (!isInLoopThread()) {
                    logAndAbort();
                }
            }

            void logAndAbort() {
                std::cerr << "running thread with id:" << tinyNL::net::CurrentThread::tid()
                          << " is not the owner of event loop (ptr): "
                          << this
                          << " which should be "
                          << threadId_
                          << std::endl;
                _exit(1);
            }

            void updateMultiplexer(Channel *channel);

            void runInLoopThread(const std::function<void()> &func);

            void wakeUp();

            std::shared_ptr<Timer>
            addTimerSinceNow(const std::function<void()> &task, long start, long interval, int repeat);

            std::shared_ptr<Timer>
            addTimerAbsolute(const std::function<void()> &task, long start, long interval, int repeat);

            void delTimer(const std::shared_ptr<Timer> &timer);

            void addPendingTasks(const std::vector<std::function<void()>> &);

        private:
            typedef std::vector<std::function<void()>> PendingTaskList;

            const pid_t threadId_;
            bool looping_;
            std::atomic_bool stop_;
            std::shared_ptr<tinyNL::net::Multiplexer> multiplexer;
            PendingTaskList pendingTasks;
            tinyNL::base::Mutex mutex_;
            int eventfd_;
            std::unique_ptr<Channel> channelUPtr;
            //every eventloop owns one timerqueue.
            std::shared_ptr<TimerQueue> timerQueueOfEventLoop;

            static void eventReadCallBack(int fd);

            void doPendingTask();

            int setUpEventFd();

            void queueInLoop(const std::function<void()> &function);
        };
    }
}
#endif //TINYNL_EVENTLOOP_H
