//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/EPoller.h>
#include <tinyNL/base/MutexLockGuard.h>
#include <tinyNL/net/Multiplexer.h>
#include <tinyNL/net/Channel.h>
#include <tinyNL/net/TimerQueue.h>
#include <iostream>
#include <assert.h>
#include <sys/eventfd.h>
#include <tinyNL/base/Log.h>
#include <tinyNL/base/TimeUtilies.h>
#include "Timer.h"

namespace tinyNL {
    namespace net {
        thread_local EventLoop *threadEventLoopPtr = nullptr;

        EventLoop::EventLoop()
                : threadId_(tinyNL::net::CurrentThread::tid()),
                  looping_(false),
                  stop_(false),
                  multiplexer(new EPoller()),
                  eventfd_(setUpEventFd()),
                  channelUPtr(new Channel(eventfd_, this)),
                  timerQueueOfEventLoop(new TimerQueue(this)) {
            if (threadEventLoopPtr == nullptr) {
                threadEventLoopPtr = this;
            } else {
                std::cerr << "current running thread id :" << tinyNL::net::CurrentThread::tid()
                          << " already owns one eventloop :"
                          << threadEventLoopPtr << std::endl;
            }
            //eventchannel is needed to be wake up from blocking on multiplexer
            //set it up here

            //avoid clion editor "red underline" complain
            auto f = std::bind(EventLoop::eventReadCallBack, eventfd_);
            channelUPtr->setReadCallBack(f);
            channelUPtr->enableRead();
        }

//1.not working
//2.canceling one to one bounding with thread
        EventLoop::~EventLoop() {
            assert(!looping_);
            threadEventLoopPtr = nullptr;

            //deal with event channel
            channelUPtr->disableChannel();

            //deal with eventfd_
            close(eventfd_);
        }

        void EventLoop::loop() {
            assertInLoopThread();
            assert(!looping_);
            looping_ = true;
            while (!stop_) {
                //1.wait on io multiplex
                tinyNL::net::Multiplexer::ChannelList activeChannels;
                multiplexer->multiplexerPolling(4000, activeChannels);
                for (auto iter : activeChannels) {
                    iter->handleEvent();
                }

                //2.deal with pending functions
                doPendingTask();
            }
            looping_ = false;
        }

        void EventLoop::stop() {
            stop_ = true;
            if (!isInLoopThread()) {
                wakeUp();
            }
        }

        void EventLoop::updateMultiplexer(Channel *channel) {
            assert(channel->getEventLoop() == this);
            assertInLoopThread();
            multiplexer->multiplexerUpdate(channel);
        }

        void EventLoop::doPendingTask() {
            assertInLoopThread();
            PendingTaskList local;
            {
                base::MutexLockGuard m(mutex_);
                std::swap(local, pendingTasks);
            }
            for (auto &iter : local) {
                iter();
            }
        }

        void EventLoop::runInLoopThread(const std::function<void()> &func) {
            if (isInLoopThread()) {
//                std::cout<<"\nrunInLoopThread direct run" <<std::endl;

                func();
            } else {
//                std::cout<<"\nrunInLoopThread queue up " <<std::endl;

                queueInLoop(func);
                wakeUp();
            }
        }

        void EventLoop::queueInLoop(const std::function<void()> &function) {
            {
                base::MutexLockGuard m(mutex_);
                pendingTasks.push_back(function);
            }
        }

        int EventLoop::setUpEventFd() {
            //initial state is not readable
            int ret = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
            if (ret == -1) {
                base::LOG.logErrorAndExit();
            }
            return ret;
        }

        void EventLoop::eventReadCallBack(int fd) {
            uint64_t msg;
            ssize_t ret = read(fd, &msg, sizeof(msg));
            if (ret == -1 && errno != EINTR) {
                base::LOG.logErrorAndExit();
            }
        }

        void EventLoop::wakeUp() {
            uint64_t msg = 1;
            ssize_t ret = write(eventfd_, &msg, sizeof(msg));
            if (ret == -1 && errno != EINTR) {
                base::LOG.logErrorAndExit();
            }
        }

        void EventLoop::addPendingTasks(const std::vector<std::function<void()>> &tasks) {
            {
                base::MutexLockGuard m(mutex_);
                std::copy(tasks.begin(), tasks.end(), std::back_inserter(pendingTasks));
            }
        }

        std::shared_ptr<Timer> EventLoop::addTimerSinceNow(const std::function<void()> &task, long start, long interval, int repeat) {
            std::shared_ptr<Timer> timer(
                    new Timer(task, tinyNL::base::TimeUtilies::millionSecondsSinceEposh() + start, interval, repeat));
            timerQueueOfEventLoop->addTimer(timer);
            return timer;
        }

        std::shared_ptr<Timer> EventLoop::addTimerAbsolute(const std::function<void()> &task, long start, long interval, int repeat) {
            std::shared_ptr<Timer> timer(new Timer(task, start, interval, repeat));
            timerQueueOfEventLoop->addTimer(timer);
            return timer;
        }

        void EventLoop::delTimer(const std::shared_ptr<Timer> &timer) {
            timerQueueOfEventLoop->delTimer(timer);
        }
    }
}