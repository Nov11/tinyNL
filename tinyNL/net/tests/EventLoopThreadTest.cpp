//
// Created by c6s on 17-2-24.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/EventLoopThread.h>

using namespace tinyNL;
using namespace tinyNL::net;

void onMsg(const std::shared_ptr<TcpConnection>& con) {
    Buffer &buffer = con->readBuf;
    char *ptr = buffer.readPtr();
    size_t len = buffer.readableSize();
    std::string s(ptr, len);
    buffer.erase(len);
    std::cout << "usercode onMsg: " << s << std::endl;
    std::string str("hello from srv");
    con->send(str);
}

void fff(EventLoop* loop){
    std::shared_ptr<TcpServer> server = std::make_shared<TcpServer>(loop, 60000);
    server->setOnMessageCallBack(::onMsg);
    server->start();
    loop->loop();
}
int main(){
    EventLoopThread th(fff);
    EventLoop * loopPtr = th.start();
    std::cout<<"loopPtr:"<<loopPtr<<std::endl;
    sleep(20);
//    loopPtr->stop();
    std::cout<<"main return"<<std::endl;
}