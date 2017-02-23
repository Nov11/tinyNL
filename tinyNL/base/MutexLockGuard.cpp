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
            mutex_.lock();
        }

        MutexLockGuard::~MutexLockGuard() {
            mutex_.unlock();
        }
    }
}