//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_SOCKET_H
#define TINYNL_SOCKET_H

#include <tinyNL/dir/Noncopyable.h>
#include <netinet/in.h>

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
            void setReuseAddr(bool on);
            int accept4(struct sockaddr_in *peer);
            void bind(int port);
            void listen();
            int connect(const sockaddr_in&);
            static int getNonBlockingTcpSocket();
            int socket()const {return socket_;}
        private:
            int socket_;
        };
    }

}
#endif //TINYNL_SOCKET_H
