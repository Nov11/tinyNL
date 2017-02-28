//
// Created by c6s on 17-2-28.
//

//echo, time, daytine, discard, chargen
//all in one eventloop of same thread


#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>

using namespace tinyNL::net;
using namespace tinyNL::base;
#include <sstream>
#include <netinet/in.h>


void onMsgEchoSrv(const std::shared_ptr<TcpConnection>& con){
    std::string s = con->read();
    std::string tmp;
    tmp.append("echo Srv. peer message: ").append(s);
    LOG<<tmp;
    con->send(s);
}

void onConnectionDayTimerSrv(const std::shared_ptr<TcpConnection> &con) {
    std::stringstream ss;
    ss << "DayTime Server:" << " peer " << AddressUtility::toString(con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();
    time_t t = ::time(nullptr);

    std::string time(::ctime(&t));
    con->send(time);
    con->closeConnection();
}

void onMessageDiscardSrv(const std::shared_ptr<TcpConnection>& con) {
    std::string msg = con->read();
    std::stringstream ss;
    ss << "discard :" << msg << " from " << AddressUtility::toString( con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();
}

std::string charseq = "~!@#$%^&*()_+`1234567890-=abcdefghijklmnopqrstuvwxyz\\][';/.,|}{\":?><:";
void onWriteCompleteOfCharGen(const std::shared_ptr<TcpConnection>& con){
    con->send(charseq);
}

void onConnectionTimeSrv(const std::shared_ptr<TcpConnection> &con) {
    std::stringstream ss;
    ss << "Time Server:" << " peer " << AddressUtility::toString(con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();

    //string convoies u32int in binary, which is 4 bytes.
    time_t t = ::time(nullptr);
    uint32_t u32 = static_cast<uint32_t>(t);
    std::string time(reinterpret_cast<char*>(&u32), sizeof(u32));
    LOG<<std::to_string(u32);
    con->send(time);
    con->closeConnection();
}

int main(){
    LOG << "main started";
    EventLoop loop;
    std::shared_ptr<TcpServer> timeSrv(new TcpServer(&loop, 60000));
    timeSrv->setOnConnectionCallBack(::onConnectionTimeSrv);
    timeSrv->start();

    std::shared_ptr<TcpServer> discardSrv(new TcpServer(&loop, 60001));
    discardSrv->setOnMessageCallBack(::onMessageDiscardSrv);
    discardSrv->start();

    std::shared_ptr<TcpServer> daytimeSrv(new TcpServer(&loop, 60002));
    daytimeSrv->setOnConnectionCallBack(::onConnectionDayTimerSrv);
    daytimeSrv->start();

    std::shared_ptr<TcpServer> chargenSrv(new TcpServer(&loop, 60003));
    chargenSrv->setOnWriteComplete(::onWriteCompleteOfCharGen);
    chargenSrv->start();

    std::shared_ptr<TcpServer> echoSrv(new TcpServer(&loop, 60004));
    echoSrv->setOnMessageCallBack(::onMsgEchoSrv);
    echoSrv->start();

    loop.addTimerSinceNow([&loop](){loop.stop();}, 60 * 1000, 0, 0);

    loop.loop();
    timeSrv->demolish();
    discardSrv->demolish();
    daytimeSrv->demolish();
    chargenSrv->demolish();
    echoSrv->demolish();
    //no calling demolish on srv
    LOG << "main return";
}