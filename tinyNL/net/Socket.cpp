//
// Created by c6s on 17-2-21.
//
#include <tinyNL/net/Socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <tinyNL/base/Log.h>

using namespace tinyNL::net;


void Socket::setTcpNoDelay(bool on) {
    int flag = on ? 1 : 0;
    ::setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

Socket::~Socket() {
    close(socket_);
}

void Socket::setNoBlocking(bool on) {
    int flags = fcntl(socket_, F_GETFL, nullptr);
    if(flags < 0){
        tinyNL::base::LOG.logErrorAndExit();
    }
    if(on){
        flags |= O_NONBLOCK;
    }else{
        flags &= ~O_NONBLOCK;
    }
    int ret = fcntl(socket_, F_SETFL, flags);
    if(ret < 0){
        tinyNL::base::LOG.logErrorAndExit();
    }
}
