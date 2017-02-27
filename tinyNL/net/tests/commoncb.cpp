// Created by c6s on 17-2-25.
//

#include <tinyNL/net/tests/commonccb.h>
#include <tinyNL/base/Log.h>
#include <sstream>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoop.h>

void onMessage(const std::shared_ptr<tinyNL::net::TcpConnection>&con) {
    std::string tmp;
    tmp.append("usercode onMsg: ").append(con->read());
    tinyNL::base::LOG<<tmp;
    std::string str("hello from this");
    con->send(str);
//    con->closeConnection();
}

void onConnection(const std::shared_ptr<tinyNL::net::TcpConnection>& con) {
    std::stringstream ss;
    ss << "usercode onCon new connection established. peer: " << tinyNL::net::AddressUtility::toString(con->peerAddr())
       << " port: "
       << ntohs(con->peerAddr().sin_port)
       <<std::endl;
    tinyNL::base::LOG<<ss.str();
}

void onPeerClose(const std::shared_ptr<tinyNL::net::TcpConnection>& con) {
    std::stringstream ss;
    ss<< "usercode onClose peer close :" << tinyNL::net::AddressUtility::toString(con->peerAddr())
      << " port: "
      << ntohs(con->peerAddr().sin_port)
      << std::endl;
    tinyNL::base::LOG<<ss.str();
}


void fail(){
    tinyNL::base::LOG<<"connection failed\n";
}

void suc(int fd){
    tinyNL::base::LOG<<"suc";
}

void infinitLoop(tinyNL::net::EventLoop* loop_){
    loop_->loop();
}