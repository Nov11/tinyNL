//
// Created by c6s on 17-2-21.
//

#include <pthread.h>
#include <cstring>
#include "Mutex.h"
#include "Log.h"

namespace tinyNL{
    namespace base{
        tinyNL::base::Mutex::Mutex() {
            int ret = pthread_mutex_init(&mutex_, NULL);
            if(ret != 0){
                tinyNL::base::LOG << strerror(errno);
            }
        }

        Mutex::~Mutex() {
            int ret = pthread_mutex_destroy(&mutex_);
            if(ret != 0){
                tinyNL::base::LOG << strerror(errno);
            }
        }
    }
}

