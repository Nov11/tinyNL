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
            explicit EventLoopThread(const FunctionRunningInThread& tf);//well, seems that here needs a default tf
            EventLoopThread();
            ~EventLoopThread(){
                stop();
                thread_.join();}
            EventLoop* start();
            void join(){ thread_.join(); }
        private:
            FunctionRunningInThread tf_;
            base::Thread thread_;
            EventLoop* loop_;
            base::Mutex mutex_;
            base::Cond cond;
            void eventLoopThread();
            static void defaultThreadFunc(EventLoop* eventLoop);
            //<del>1.do not add call to stop into destor
            //for now, stop should not be placed into destor.
            //suppose this obj locate on main's stack, and thread say TH ,
            // that is started by this obj is blocked in io multiplexer.
            //main returns, destor called, which will call stop on the eventloop on TH,
            // and destor returns, this obj is gone later.
            //loop on TH wake up, gonna use some obj refs which bound in obj tf_ and lived on main thread.
            //but objs have already gone since tf_ on main thread destructed.
            //given that tf_ is user function, this could happen.
            //so, do not use stop in destor. even if it looks attractive.</del>
            // call to join ensures this obj lives longer than 'thread_'.former situation will not happen
            //2.<del>stop should be called after all connections running in this thread closed.
            //or it cannot be guaranteed that close call back will run. maybe a counter is needed?
            //em, make it one restriction that tcp server uses this obj stops running first.
            //if user do not hold ref of tcpconnection for longer life time range than eventloopthread,
            //invocation of onclose can be guaranteed.</del>
            //make sure close callback on tcpconnection in invoked.
            //cases:1. 'stop' invoked when tcp connection not closed.
            //         depends on  tcp server or connection calling close before stop is invoked.
            //         eventloop can never tell what objs are running on it and it doesn't care
            //         #this answers who calls 'close' on tcp connection
            //      2. 'stop' sets bool value just after pending function processing returned.
            //         call dopendingfunctions again after while in loop function.
            //         at this moment, nothing except one bool value(stop_) is changed, environment is just like that
            //         in the while loop. thus it's safe. and can solve the problem.
            //
            void stop();
        };
    }
}



#endif //TINYNL_EVENTLOOPTHREAD_H
