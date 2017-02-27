//
// Created by c6s on 17-2-27.
//

//
// Created by c6s on 17-2-27.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>

using namespace tinyNL::net;
using namespace tinyNL::base;

void onConnection(const std::shared_ptr<TcpConnection> &con) {
    std::string msg = con->read();
    std::stringstream ss;
    ss << "Time Server:" << " peer " << AddressUtility::toString(con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();
    time_t t = ::time(nullptr);
    uint32_t u32 = static_cast<uint32_t>(t);
    std::string time(reinterpret_cast<char*>(&u32), sizeof(u32));
    LOG<<std::to_string(u32);
    con->send(time);
    con->closeConnection();
}

int main() {
    LOG << "enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000));
    //set time callback
    srvptr->setOnConnectionCallBack(::onConnection);
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