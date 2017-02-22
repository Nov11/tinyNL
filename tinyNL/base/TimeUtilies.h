//
// Created by c6s on 17-2-22.
//

#ifndef TINYNL_TIMEUTILIES_H
#define TINYNL_TIMEUTILIES_H

#include <sys/time.h>

namespace tinyNL{
    namespace base{
        class TimeUtilies {
        public:
            static long millionSecondsSinceEposh(){
                timeval tv;
                ::gettimeofday(&tv, nullptr);
                return tv.tv_sec * 1000 + tv.tv_usec / 1000;
            }
        };
    }
}



#endif //TINYNL_TIMEUTILIES_H
