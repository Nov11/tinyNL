//
// Created by c6s on 17-2-21.
//
#include <tinyNL/net/CurrentThread.h>

namespace tinyNL{
    namespace net{
        thread_local pid_t currentThreadId = 0;

        pid_t CurrentThread::tid() {
            if (currentThreadId == 0) {
                currentThreadId = static_cast<pid_t >(::syscall(SYS_gettid));
            }
            return currentThreadId;
        }
    }
}
