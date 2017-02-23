//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_MUTEXLOCKGUARD_H
#define TINYNL_MUTEXLOCKGUARD_H

#include "Mutex.h"

namespace tinyNL {
    namespace base {

        class MutexLockGuard : Noncopyable{
        public:
            explicit MutexLockGuard(Mutex& mutex);
            ~MutexLockGuard();
        private:
            Mutex& mutex_;
        };
    }

}


#endif //TINYNL_MUTEXLOCKGUARD_H
