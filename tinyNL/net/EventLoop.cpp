//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/EPoller.h>
#include <tinyNL/base/MutexLockGuard.h>
#include <tinyNL/net/Multiplexer.h>
#include <tinyNL/net/Channel.h>
#include <iostream>
#include <assert.h>

namespace tinyNL {
    namespace net {
        thread_local EventLoop *threadEventLoopPtr = nullptr;

        EventLoop::EventLoop()
                : threadId_(tinyNL::net::CurrentThread::tid()),
                  looping_(false),
                  stop_(false),
                  multiplexer(new EPoller()) {
            if (threadEventLoopPtr == nullptr) {
                threadEventLoopPtr = this;
            } else {
                std::cerr << "current running thread id :" << tinyNL::net::CurrentThread::tid()
                          << " already owns one eventloop :"
                          << threadEventLoopPtr << std::endl;
            }
        }

//1.not working
//2.canceling one to one bounding with thread
        EventLoop::~EventLoop() {
            assert(!looping_);
            threadEventLoopPtr = nullptr;
        }

        void EventLoop::loop() {
            assertInLoopThread();
            assert(!looping_);
            looping_ = true;
            while (!stop_) {
            //1.wait on io multiplex
            tinyNL::net::Multiplexer::ChannelList activeChannels;
            multiplexer->multiplexerPolling(5000, activeChannels);
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
        }

        void EventLoop::updateMultiplexer(Channel *channel) {
            assert(channel->getEventLoop() == this);
            assertInLoopThread();
            multiplexer->multiplexerUpdate(channel);
        }

        void EventLoop::doPendingTask() {
            PendingTaksList local;
            {
                base::MutexLockGuard m(mutex_);
                std::swap(local, pendingTasks);
            }
            for (auto &iter : local) {
                iter();
            }
        }

    }
}