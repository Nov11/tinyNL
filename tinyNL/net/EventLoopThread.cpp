//
// Created by c6s on 17-2-24.
//

#include <tinyNL/base/MutexLockGuard.h>
#include "EventLoopThread.h"
#include "EventLoop.h"

namespace tinyNL {
    namespace net {

        EventLoopThread::EventLoopThread(const EventLoopThread::FunctionRunningInThread& tf)
                : tf_(tf),
                  thread_(std::bind(&EventLoopThread::eventLoopThread, this)),
                  loop_(nullptr),
                  cond(mutex_) {
        }

        EventLoop *EventLoopThread::start() {
            assert(thread_.isRunning() == false);
            thread_.run();
            {
                base::MutexLockGuard guard(mutex_);
                while (loop_ == nullptr) {
                    cond.wait();
                }
            }
            return loop_;
        }

        void EventLoopThread::eventLoopThread() {
            EventLoop loop;
            {
                base::MutexLockGuard guard(mutex_);
                loop_ = &loop;
                cond.notify();
            }
            tf_(&loop);
        }

        void EventLoopThread::stop() {
            loop_->stop();
        }

        EventLoopThread::EventLoopThread() :EventLoopThread(&EventLoopThread::defaultThreadFunc){

        }

        //use this for default constructor only, do not use it elsewhere.
        //it starts loop. the function servers as tf_ in eventLoopThread function which just lies above;
        void EventLoopThread::defaultThreadFunc(EventLoop *eventLoop) {
            eventLoop->loop();
        }


    }
}