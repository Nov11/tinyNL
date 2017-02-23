//
// Created by c6s on 17-2-23.
//


#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/Acceptor.h>
#include <signal.h>
#include <tinyNL/base/Log.h>

using namespace tinyNL;
using namespace tinyNL::net;
EventLoop eventLoop;
void stopEventLoop(int){
    std::cout<<"stopEventLoop"<<std::endl;
    eventLoop.stop();
}
int main(){

    auto r = signal(SIGQUIT, ::stopEventLoop);
    if(r == SIG_ERR){
        base::LOG.logErrorAndExit();
    }
    Acceptor::AcceptorUserCallBack cb = [](int fd, sockaddr_in){std::cout<<"exec acceptor call back" <<std::endl;
        close(fd);std::cout<<"closed incoming connection" <<std::endl;};
    Acceptor acceptor(&eventLoop, 60000, cb);
    eventLoop.loop();
}