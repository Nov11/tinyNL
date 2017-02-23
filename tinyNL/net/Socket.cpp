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
#include <cstring>

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
    if (flags < 0) {
        tinyNL::base::LOG.logErrorAndExit();
    }
    if (on) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    int ret = fcntl(socket_, F_SETFL, flags);
    if (ret < 0) {
        tinyNL::base::LOG.logErrorAndExit();
    }
}

int Socket::getNonBlockingTcpSocket() {
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (ret == -1) {
        base::LOG.logErrorAndExit();
    }
    return ret;
}

void Socket::bind(int port) {
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t >(port));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = ::bind(socket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (ret == -1) {
        base::LOG.logErrorAndExit();
    }
}

int Socket::accept4(struct sockaddr_in *peer) {
    socklen_t len = sizeof(sockaddr_in);
    int ret = ::accept4(socket_, reinterpret_cast<sockaddr *>(peer), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (ret == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //it's ok
        } else {
            //not likely to see econnaborted
            base::LOG.logError();
        }
    }
    return ret;
}

void Socket::listen() {
    int ret = ::listen(socket_, 128);
    if (ret == -1) {
        base::LOG.logErrorAndExit();
    }
}

int Socket::connect(const sockaddr_in &peer) {
    socklen_t len = sizeof(sockaddr_in);
    int ret = ::connect(socket_, reinterpret_cast<sockaddr *>(
            const_cast<sockaddr_in *>(&peer)
    ), len);
    if (ret == -1) {
        if (ret == EINPROGRESS) {
            //it's ok
        } else {
            base::LOG.logErrorAndExit();
        }
    }
    return ret;
}

void Socket::setReuseAddr(bool on) {
    int op = on ? 1 : 0;
    int ret = ::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
    if (ret == -1) {
        base::LOG.logErrorAndExit();
    }
}


