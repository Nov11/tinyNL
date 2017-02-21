//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_CURRENTTHREAD_H
#define TINYNL_CURRENTTHREAD_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

namespace tinyNL {
    namespace net {
        class CurrentThread {
        public:
            static pid_t tid();
        };
    }

}
#endif //TINYNL_CURRENTTHREAD_H
