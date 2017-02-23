//
// Created by c6s on 17-2-23.
//

#include <tinyNL/net/Socket.h>
#include <cstring>
#include <tinyNL/base/Log.h>
#include <iostream>

using namespace tinyNL;
using namespace tinyNL::net;
int main(){
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    Socket socket(fd);
    socket.setReuseAddr(true);
    socket.bind(60001);
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(60000);
    int ret = socket.connect(addr);
    if(ret == -1){ base::LOG.logErrorAndExit(); }
    int end;
    std::cout<<"before reading"<<std::endl;
    ssize_t red = read(fd, &end, sizeof(end));
    std::cout<<"red: " << red << " end:" << end<<std::endl;
}