//
// Created by c6s on 17-2-21.
//

#include <tinyNL/base/Log.h>
#include <cstring>
#include <tinyNL/net/CurrentThread.h>

namespace tinyNL{
    namespace base{
        Log LOG;
        Log& Log::operator<<(const std::string& str) {
            std::string inner = "[#thread no: ";
            inner.append(std::to_string(net::CurrentThread::tid()));
            inner.append(" ");
            inner.append(str);
            inner.append("\n");
            write(outputfd_, inner.c_str(), strlen(inner.c_str()));
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

        void Log::logError(int param) {
            std::string msg(strerror(param));
            *this << msg;
        }

    }
}
