//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_THREAD_H
#define TINYNL_THREAD_H

#include <sys/types.h>
#include <tinyNL/dir/Noncopyable.h>
#include <functional>
#include <atomic>
#include <assert.h>
#include "Log.h"

namespace tinyNL {
    namespace base {
        class Thread : Noncopyable {
        public:
            explicit Thread(const std::function<void()> &callBack)
                    : threadId_(0),
                      cb_(callBack),
                      running_(false) {

            }

            ~Thread() {
                //thread should have stopped running when this object destructed
                assert(running_ == false);
            }

            void run() {
                auto f =  std::bind(&Thread::threadBookKeeping, this);
                task = f;
                if (pthread_create(&threadId_, nullptr, &Thread::runCallBack, &task) != 0) {
                    LOG.logErrorAndExit();
                }
            }

            void join() {
                int ret = pthread_join(threadId_, nullptr);
                if (ret != 0) {
                    LOG.logErrorAndExitPthread(ret);
                }
                threadId_ = 0;
            }

            bool isRunning(){return running_;}
        private:
            pthread_t threadId_;
            const std::function<void()> cb_;
            std::atomic_bool running_;
            std::function<void()> task;
            void threadBookKeeping(){
                running_ = true;
                cb_();
                running_ = false;
            }

            static void *runCallBack(void *cb) {
                std::function<void()> *f = reinterpret_cast<std::function<void()> *>(cb);
                (*f)();
                return nullptr;
            }
        };
    }
}


#endif //TINYNL_THREAD_H
