//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_TIMER_H
#define TINYNL_TIMER_H

#include <functional>

namespace tinyNL{
    namespace net{
        //objects of this class are copyable
        class Timer {
        public:
            typedef std::function<void()> TimerCallBack;
            Timer(TimerCallBack tcb)
            :callBack(tcb){}
            Timer(long start){}
            ~Timer(){};
        private:
            std::function<void()> callBack;
            long interval_;
            int repeat_;
            long start_;
        };
    }
}



#endif //TINYNL_TIMER_H
