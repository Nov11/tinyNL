//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_SOCKET_H
#define TINYNL_SOCKET_H

#include <tinyNL/dir/Noncopyable.h>

namespace tinyNL{
    namespace net{
        class Socket : Noncopyable {
        public :
            explicit Socket(int fd)
                    :socket_(fd){

            }
            ~Socket();
            void setTcpNoDelay(bool on);
            void setNoBlocking(bool on);

        private:
            int socket_;
        };
    }

}
#endif //TINYNL_SOCKET_H
