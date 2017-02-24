//
// Created by c6s on 17-2-24.
//


#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/EventLoopThread.h>
#include <thread>

using namespace tinyNL;
using namespace tinyNL::net;

EventLoop loop;

void func(int n) {
    sleep(n);
    loop.stop();
}


int main() {
    std::thread th(func, 10);
    loop.loop();
    th.join();
    std::cout<<"main return"<<std::endl;
}