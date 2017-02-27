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
#include <sstream>
#include <tinyNL/net/EventLoopThread.h>

using namespace tinyNL;
using namespace tinyNL::net;

EventLoop loop;

std::shared_ptr<TcpServer> tcpServer(new TcpServer(&loop, 60000));
std::shared_ptr<TcpConnection> chkptr;

void fff(const std::shared_ptr<TcpConnection>& con) {
    Buffer &buffer = con->readBuf;
    char *ptr = buffer.readPtr();
    size_t len = buffer.readableSize();
    std::string s(ptr, len);
    buffer.erase(len);
//    chkptr = con;
//    std::cout << "usercode onMsg: " << s << std::endl;
    std::string tmp;
    tmp.append("usercode onMsg: ").append(s);
    base::LOG<<tmp;
    std::string str("hello from srv");
    con->send(str);
//    con->closeConnection();
}

void onConnection(const std::shared_ptr<TcpConnection>& con) {
    std::stringstream ss;
    ss << "usercode onCon new connection from host: " << AddressUtility::toString(con->peerAddr())
              << " port: "
              << ntohs(con->peerAddr().sin_port)
            <<std::endl;
//    base::LOG<<ss.str();
}

void onPeerClose(const std::shared_ptr<TcpConnection>& con) {
    std::stringstream ss;
    ss<< "usercode onClose peer close :" << AddressUtility::toString(con->peerAddr())
                                                 << " port: "
                                                 << ntohs(con->peerAddr().sin_port)
                                                 << std::endl;
    ss<<"usercode onClose peer close chkptr use_count: "<<chkptr.use_count()<<std::endl;
//    base::LOG<<ss.str();
}

void stopEventLoop(uint32_t sec) {
    sleep(sec);
    std::stringstream ss;
    ss<< "stopEventLoop" << std::endl;
    base::LOG<<ss.str();
    loop.stop();
}

//void stopTcpServer(TcpServer* tcp){
//    tcp->demolish();
//    std::cout<<chkptr.use_count()<<std::endl;
//}

void stopTcpSrvInanoterThread(){
    sleep(20);
    tcpServer->demolish();
}
void infi(EventLoop* loop_){
    loop_->loop();
}

EventLoopThread eventLoopThread(infi);
int main() {
    base::LOG<<"enter main\n";
    using CB = std::function<void(const std::shared_ptr<TcpConnection>&)>;
    CB cb(::fff);
    CB cb2(::onConnection);
    CB cb3(::onPeerClose);
    tcpServer->setOnMessageCallBack(cb);
    tcpServer->setOnConnectionCallBack(cb2);
    tcpServer->setOnPeerCloseCallBack(cb3);

    EventLoop* loop1 = eventLoopThread.start();
    tcpServer->addWorkEventLoop(loop1);
    tcpServer->start();
//    loop.addTimerSinceNow([&](){stopTcpServer(&tcpServer);}, 6* 1000, 0, 0);

    std::thread th(::stopTcpSrvInanoterThread);
    std::thread th1(::stopEventLoop, 30);

    loop.loop();
//    loop1->stop();
    th.join();
    th1.join();
    std::cout<<"chkptr use_count: "<<chkptr.use_count()<<std::endl;
    std::cout << "main return" << std::endl;
}