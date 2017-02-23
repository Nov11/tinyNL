//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_COND_H
#define TINYNL_COND_H

#include <sys/types.h>
#include "Mutex.h"

namespace tinyNL{
    namespace base{
        class Cond :Noncopyable{
        public:
            explicit Cond(Mutex& mutex);
            ~Cond();
            void wait();
            void notify();
            void notifyAll();
        private:
            pthread_cond_t cond_;
            Mutex& mutex_;
        };

    }
}


#endif //TINYNL_COND_H
