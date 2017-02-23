//
// Created by c6s on 17-2-23.
//

#ifndef TINYNL_IGNORESIGPIPE_H
#define TINYNL_IGNORESIGPIPE_H


#include <signal.h>
#include "Log.h"

namespace tinyNL {
    namespace base {
        class IgnoreSIGPIPE :Noncopyable{
        public:
            IgnoreSIGPIPE() {
                if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
                    tinyNL::base::LOG.logErrorAndExit();
                }
            }
        };
    }

}


#endif //TINYNL_IGNORESIGPIPE_H
