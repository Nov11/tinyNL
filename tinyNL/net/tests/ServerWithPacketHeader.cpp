//
// Created by c6s on 17-10-19.
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
void infinitLoop(EventLoop* loop_){
    loop_->loop();
}



void onMsg(const std::shared_ptr<TcpConnection>& con){
    base::LOG<<"onMsg";
    size_t msgSize = con->readableBytes();
    if(msgSize < sizeof(uint32_t)){
        return;
    }
    auto ret = con->readBuf.peek(sizeof(uint32_t));
    uint32_t len = 0;
    memcpy(&len, &ret[0], sizeof(uint32_t));
    len = ntohl(len);
    size_t packetLen = sizeof(uint32_t) + len;
    if(msgSize < packetLen){
        return;
    }
    std::string s = con->read(packetLen);
    for(auto item : s){
        std::cout << std::hex << item;
    }
    std::cout << std::endl;
}

void onConnection(const std::shared_ptr<TcpConnection>& con) {
    std::stringstream ss;
    ss << "usercode onCon new connection from host: " << AddressUtility::toString(con->peerAddr())
       << " port: "
       << ntohs(con->peerAddr().sin_port)
       <<std::endl;
    base::LOG<<ss.str();
}

void onPeerClose(const std::shared_ptr<TcpConnection>& con) {
    std::stringstream ss;
    ss<< "usercode onClose peer close :" << AddressUtility::toString(con->peerAddr())
      << " port: "
      << ntohs(con->peerAddr().sin_port)
      << std::endl;
    base::LOG<<ss.str();
}

void shut(int delay){
    sleep(delay);
    loop.stop();
}
int main(){
    std::thread th(::shut, 600);
    EventLoopThread elt1(infinitLoop);
    EventLoopThread elt2(infinitLoop);

    base::LOG<<"main thread";
    //basic setup
    std::shared_ptr<TcpServer> server(new TcpServer (&loop, 60000));
    server->setOnPeerCloseCallBack(::onPeerClose);
    server->setOnMessageCallBack(::onMsg);
    server->setOnConnectionCallBack(::onConnection);

    //setup working thread
    server->addWorkEventLoop(elt1.start());
    server->addWorkEventLoop(elt2.start());

    server->start();
    loop.loop();


    server->demolish();
//    elt1.stop();
//    elt2.stop();
    th.join();
    base::LOG<<"return from main\n";
}