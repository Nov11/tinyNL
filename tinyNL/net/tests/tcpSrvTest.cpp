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
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <thread>

using namespace tinyNL;
using namespace tinyNL::net;

EventLoop loop;
TcpServer tcpServer(&loop, 60000);
std::shared_ptr<TcpConnection> chkptr;

void fff(std::shared_ptr<TcpConnection> con) {
    Buffer &buffer = con->readBuf;
    char *ptr = buffer.readPtr();
    size_t len = buffer.readableSize();
    std::string s(ptr, len);
    buffer.erase(len);
//    chkptr = con;
    std::cout << "usercode onMsg: " << s << std::endl;
    std::string str("hello from srv");
    con->send(str);
    con->closeConnection();
}

void onConnection(std::shared_ptr<TcpConnection> con) {
    std::cout << "usercode onCon new connection from host: " << AddressUtility::toString(con->peerAddr())
              << " port: "
              << ntohs(con->peerAddr().sin_port)
              << std::endl;
}

void onPeerClose(std::shared_ptr<TcpConnection> con) {
    std::cout << "usercode onClose peer close :" << AddressUtility::toString(con->peerAddr())
                                                 << " port: "
                                                 << ntohs(con->peerAddr().sin_port)
                                                 << std::endl;
    std::cout<<"usercode onClose peer close chkptr use_count: "<<chkptr.use_count()<<std::endl;
}

void stopEventLoop(uint32_t sec) {
    sleep(sec);
    std::cout << "stopEventLoop" << std::endl;
    loop.stop();
}

void stopTcpServer(TcpServer* tcp){
    tcp->demolish();
    std::cout<<chkptr.use_count()<<std::endl;
}

void stopTcpSrvInanoterThread(){
    sleep(10);
    tcpServer.demolish();
}

int main() {
    std::cout<<"enter main" <<std::endl;
    std::function<void(std::shared_ptr<TcpConnection>)> cb(fff);
    std::function<void(std::shared_ptr<TcpConnection>)> cb2(::onConnection);
    std::function<void(std::shared_ptr<TcpConnection>)> cb3(::onPeerClose);
    tcpServer.setOnMessageCallBack(cb);
    tcpServer.setOnConnectionCallBack(cb2);
    tcpServer.setOnPeerCloseCallBack(cb3);
    tcpServer.start();
//    loop.addTimerSinceNow([&](){stopTcpServer(&tcpServer);}, 6* 1000, 0, 0);

    std::thread th(::stopTcpSrvInanoterThread);
    std::thread th1(::stopEventLoop, 20);

    loop.loop();
    th.join();
    th1.join();
    std::cout<<"chkptr use_count: "<<chkptr.use_count()<<std::endl;
    std::cout << "main return" << std::endl;
}