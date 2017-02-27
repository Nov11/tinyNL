//
// Created by c6s on 17-2-27.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>
#include "commonccb.h"

using namespace tinyNL::net;
using namespace tinyNL::base;

void onDayTimerService(const std::shared_ptr<TcpConnection> &con) {
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

int main() {
    LOG << "enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000));
    //set daytime callback
    srvptr->setOnConnectionCallBack(::onDayTimerService);
    srvptr->start();

    //timed shutdown tcp server after 60 sec from now.
    //first disable server, then stop main thread looping.
    loop.addTimerSinceNow([&loop, &srvptr]() {
        srvptr->demolish();
        loop.stop();
    }, 60 * 1000, 0, 0);

    //main thread starts working here
    loop.loop();

    LOG << "main return";
}