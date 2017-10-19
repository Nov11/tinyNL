//
// Created by c6s on 17-10-19.
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

void init(const std::shared_ptr<TcpConnection>& con){
    std::string msg(sizeof(uint32_t), 0);
    std::string content = "client message body";
    size_t size = content.size();
    uint32_t len = static_cast<uint32_t >(size);
    len = htonl(len);
    memcpy(&msg[0], &len, sizeof(uint32_t));
    msg.append(content);
    std::cout<<"send content :" ;
    for(auto item : msg){
        std::cout << std::hex<<item;
    }
    con->send(msg);
}
int main(){
    EventLoop loop;
    sockaddr_in peer;
    peer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    peer.sin_port = htons(60000);
    peer.sin_family=AF_INET;

    std::shared_ptr<TcpClient> tcpClient(new TcpClient(&loop, peer));
    tcpClient->setConnectionFailCallBack(fail);
    tcpClient->setOnConnected(::init);
    tcpClient->setOnPeerClose(::onPeerClose);


    tcpClient->start();
    loop.addTimerSinceNow([&loop](){loop.stop();}, 60 * 1000, 0, 0);
    loop.loop();
    tcpClient->demolish();
    base::LOG<<"main return";
}