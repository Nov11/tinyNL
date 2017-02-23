//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_MUTEX_H
#define TINYNL_MUTEX_H

#include <sys/types.h>
#include <tinyNL/dir/Noncopyable.h>

namespace tinyNL{
    namespace base{
        class Mutex : Noncopyable{
        public:
            Mutex();
            ~Mutex();
            void lock();
            void unlock();
            pthread_mutex_t* getMutexPtr(){return &mutex_;}
        private:
            pthread_mutex_t mutex_;
        };
    }
}



#endif //TINYNL_MUTEX_H
