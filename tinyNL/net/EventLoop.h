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

namespace tinyNL{
    namespace net{
        class Channel;
        class Multiplexer;
        class TimerQueue;
        class EventLoop : Noncopyable{
        public:
            EventLoop();
            ~EventLoop();
            void loop();
            void stop();

            bool isInLoopThread() const {
                return threadId_ == tinyNL::net::CurrentThread::tid();
            }

            void assertInLoopThread(){
                if(!isInLoopThread()){
                    logAndAbort();
                }
            }
            
            void logAndAbort(){
                std::cerr << "running thread with id:" << tinyNL::net::CurrentThread::tid()
                                         << " is not the owner of event loop (ptr): "
                                         << this
                                         << " which should be "
                                         << threadId_
                                         << std::endl;
                _exit(1);
            }

            void updateMultiplexer(Channel* channel);

        private:
            const pid_t threadId_;
            bool looping_;
            std::atomic_bool stop_;
            std::shared_ptr<tinyNL::net::Multiplexer> multiplexer;
            typedef std::vector<std::function<void()>> PendingTaksList;
            PendingTaksList pendingTasks;
            void doPendingTask();
            tinyNL::base::Mutex mutex_;
            //every eventloop owns one timerqueue
            std::shared_ptr<TimerQueue> timerQueue;
        };
    }
}
#endif //TINYNL_EVENTLOOP_H
