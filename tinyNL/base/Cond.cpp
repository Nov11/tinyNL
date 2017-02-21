//
// Created by c6s on 17-2-21.
//

#include <pthread.h>
#include "Cond.h"
#include "Log.h"

namespace tinyNL {
    namespace base {
        static void logIfErrorOccur(int ret) {
            if (ret) {
                LOG.logErrorPthread(ret);
            }
        }

        Cond::Cond(Mutex &mutex)
                : mutex_(mutex) {
            logIfErrorOccur(pthread_cond_init(&cond_, nullptr));
        }

        Cond::~Cond() {
            logIfErrorOccur(pthread_cond_destroy(&cond_));
        }

        void Cond::wait() {
            logIfErrorOccur(pthread_cond_wait(&cond_, mutex_.getMutexPtr()));
        }

        void Cond::notify() {
            logIfErrorOccur(pthread_cond_signal(&cond_));
        }

        void Cond::notifyAll() {
            logIfErrorOccur(pthread_cond_broadcast(&cond_));
        }
    }
}