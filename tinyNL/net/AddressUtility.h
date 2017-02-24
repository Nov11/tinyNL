//
// Created by c6s on 17-2-24.
//

#ifndef TINYNL_ADDRESSUTILITY_H
#define TINYNL_ADDRESSUTILITY_H

#include <arpa/inet.h>
#include <tinyNL/base/Log.h>
#include <string>

namespace tinyNL {
    namespace net {
        class AddressUtility {
        public:
            static std::string toString(const sockaddr_in &addr) {
                char tmp[16] = {};
                const char *ret = ::inet_ntop(AF_INET, &addr.sin_addr, tmp, sizeof(tmp));
                if (ret == nullptr) {
                    base::LOG.logErrorAndExit();
                }
                return std::string(tmp, tmp + 16);
            }
        };
    }
}
#endif //TINYNL_ADDRESSUTILITY_H
