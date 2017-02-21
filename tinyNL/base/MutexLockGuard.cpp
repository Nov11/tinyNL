//
// Created by c6s on 17-2-21.
//

#include <pthread.h>
#include "MutexLockGuard.h"
#include "Log.h"

namespace tinyNL {
    namespace base {
        MutexLockGuard::MutexLockGuard(Mutex &mutex)
                : mutex_(mutex) {
            if(pthread_mutex_lock(mutex_.getMutexPtr())){
                LOG.logError();
            }
        }

        MutexLockGuard::~MutexLockGuard() {
            if(pthread_mutex_unlock(mutex_.getMutexPtr())){
                LOG.logError();
            }
        }
    }
}