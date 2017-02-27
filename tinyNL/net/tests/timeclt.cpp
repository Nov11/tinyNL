//
// Created by c6s on 17-2-27.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/base/TimeUtilies.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/Buffer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>
#include <tinyNL/net/TcpConnector.h>
#include <tinyNL/net/TcpClient.h>
#include <tinyNL/net/tests/commonccb.h>
#include <cstring>
using namespace tinyNL;
using namespace tinyNL::net;

void timeclt(const std::shared_ptr<TcpConnection>& con){
    if(con->readBuf.readableSize() >= sizeof(uint32_t)){
        //decode from string, which is 4 bytes
        std::string msg = con->read();
        uint32_t t = 0;
        std::memcpy(&t, msg.data(), sizeof(t));
        std::string sec = std::to_string(t);
        base::LOG<<sec;
    }else{
        //wait for the remaining parts to arrive
    }

}

int main(){
    EventLoop loop;
    sockaddr_in peer;
    peer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    peer.sin_port = htons(60000);
    peer.sin_family=AF_INET;
    TcpClient tcpClient(&loop, peer);
    tcpClient.setConnectionFailCallBack(fail);
//    tcpClient.setOnConnected(::onConnection);
    tcpClient.setOnMsgCB(::timeclt);


    tcpClient.start();
    loop.addTimerSinceNow([&loop](){loop.stop();}, 10 * 1000, 0, 0);
    loop.loop();
    tcpClient.demolish();
    base::LOG<<"main return";
}