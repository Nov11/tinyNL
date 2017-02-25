//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_LOG_H
#define TINYNL_LOG_H

#include <tinyNL/dir/Noncopyable.h>
#include <unistd.h>
#include <string>
namespace tinyNL{
    namespace base{
        class Log : Noncopyable{
        public:
            Log(){
                outputfd_ = STDERR_FILENO;
            }

            Log& operator<<(const std::string& str);
            void logError();
            void logError(int errno_);
            void logErrorPthread(int error);
            void logErrorAndExit();
            void logErrorAndExitPthread(int error);
        private:
            int outputfd_;
        };
        extern Log LOG;
    }
}



#endif //TINYNL_LOG_H
