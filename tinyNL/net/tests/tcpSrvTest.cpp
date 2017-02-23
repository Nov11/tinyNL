//
// Created by c6s on 17-2-23.
//


#include <tinyNL/net/EventLoop.h>
#include <sys/timerfd.h>
#include <tinyNL/net/Channel.h>
#include <cstring>
#include <tinyNL/base/TimeUtilies.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/Buffer.h>

using namespace tinyNL;
using namespace tinyNL::net;

EventLoop loop;
void fff(Buffer &buffer, TcpConnection &) {
    char *ptr = buffer.readPtr();
    size_t len = buffer.readableSize();
    std::string s(ptr, len);
    buffer.erase(len);
    std::cout << "fff: " << s << std::endl;
}
void stopEventLoop(int){
    std::cout<<"stopEventLoop"<<std::endl;
    loop.stop();
}
int main() {
    TcpServer tcpServer(&loop, 60000);
    std::function<void(Buffer &, TcpConnection &)> cb(fff);
    tcpServer.setOnMessageCallBack(cb);
    tcpServer.start();
    loop.loop();
    std::cout<<"main return"<<std::endl;
}