//
// Created by c6s on 17-2-25.
//

//
// Created by c6s on 17-2-25.
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
#include <tinyNL/net/TcpConnector.h>
#include <tinyNL/net/TcpClient.h>
#include <tinyNL/net/tests/commonccb.h>
using namespace tinyNL;
using namespace tinyNL::net;

int main(){
    EventLoop loop;
    sockaddr_in peer;
    peer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    peer.sin_port = htons(60000);
    peer.sin_family=AF_INET;
    std::shared_ptr<TcpClient> tcpClient(new TcpClient(&loop, peer));
    tcpClient->setConnectionFailCallBack(fail);
    tcpClient->setOnConnected(::onConnection);
    tcpClient->setOnMsgCB(::onMessage);
    tcpClient->setOnPeerClose(::onPeerClose);


    tcpClient->start();
    loop.addTimerSinceNow([&loop](){loop.stop();}, 10 * 1000, 0, 0);
    loop.loop();
    tcpClient->demolish();
    base::LOG<<"main return";
}