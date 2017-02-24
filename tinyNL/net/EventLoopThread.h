//
// Created by c6s on 17-2-24.
//

#ifndef TINYNL_EVENTLOOPTHREAD_H
#define TINYNL_EVENTLOOPTHREAD_H

#include <tinyNL/base/Thread.h>
#include <tinyNL/base/Mutex.h>
#include <tinyNL/base/Cond.h>

namespace tinyNL{
    namespace net{
        class EventLoop;
        class EventLoopThread:Noncopyable {
        public:
            typedef std::function<void(EventLoop*)> FunctionRunningInThread;
            EventLoopThread(FunctionRunningInThread tf);
            ~EventLoopThread(){thread_.join();}
            EventLoop* start();
            void join(){ thread_.join(); }
        private:
            FunctionRunningInThread tf_;
            base::Thread thread_;
            EventLoop* loop_;
            base::Mutex mutex_;
            base::Cond cond;
            void eventLoopThread();
        };
    }
}



#endif //TINYNL_EVENTLOOPTHREAD_H
