//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_COUNTDOWNLATCH_H
#define TINYNL_COUNTDOWNLATCH_H

#include "Mutex.h"
#include "Cond.h"
#include "MutexLockGuard.h"

namespace tinyNL{
    namespace base{
        class CountDownLatch :Noncopyable{
        public:
            explicit CountDownLatch(int count)
                    :cond_(mutex_),
                     count_(count){}

            void await(){
                MutexLockGuard m(mutex_);
                while(count_ != 0)cond_.wait();
            }
            void countDown(){
                MutexLockGuard m(mutex_);
                count_--;
                if(count_ == 0){
                    cond_.notifyAll();
                }
            }
        private:
            Mutex mutex_;
            Cond cond_;
            int count_;
        };
    }
}



#endif //TINYNL_COUNTDOWNLATCH_H
