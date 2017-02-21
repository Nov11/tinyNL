//
// Created by c6s on 17-2-21.
//

#include <tinyNL/base/Log.h>
#include <cstring>

namespace tinyNL{
    namespace base{
        Log LOG;
        Log& Log::operator<<(const std::string& str) {
            write(outputfd_, str.c_str(), strlen(str.c_str()));
            return *this;
        }

        void Log::logError() {
            std::string msg(strerror(errno));
            *this << msg;
        }

        void Log::logErrorPthread(int error) {
            std::string msg(strerror(error));
            *this << msg;
        }

        void Log::logErrorAndExit() {
            logError();
            _exit(1);
        }

        void Log::logErrorAndExitPthread(int error) {
            logErrorPthread(error);
            _exit(1);
        }

    }
}
