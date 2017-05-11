//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_TIMER_H
#define TINYNL_TIMER_H

#include <functional>
#include <tinyNL/dir/Noncopyable.h>

namespace tinyNL {
    namespace net {
        //objects of this class are copyable, well, not likely
        class Timer : Noncopyable {
        public:
            typedef std::function<void()> TimerCallBack;

            Timer(const TimerCallBack& tcb, long start, long interval__, int repeat)
                    : callBack_(tcb),
                      start_(start),
                      interval_(interval__),
                      repeat_(repeat) {}

            long startTime() const { return start_; };

            TimerCallBack callBack() { return callBack_; }

            bool repeatable() const { return repeat_ > 0; }

            long interval() const { return interval_; }

            void setStartTime(long start) { start_ = start; }

        private:
            std::function<void()> callBack_;
            // since epoch
            long start_;

            //measured in million seconds

            long interval_;
            int repeat_;
        };
    }
}


#endif //TINYNL_TIMER_H
