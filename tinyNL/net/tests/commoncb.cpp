// Created by c6s on 17-2-25.
//

#include <tinyNL/net/tests/commonccb.h>
#include <tinyNL/base/Log.h>
#include <sstream>
#include <tinyNL/net/AddressUtility.h>

void onMessage(std::shared_ptr<tinyNL::net::TcpConnection> con) {
    std::string tmp;
    tmp.append("usercode onMsg: ").append(con->read());
    tinyNL::base::LOG<<tmp;
    std::string str("hello from this");
    con->send(str);
//    con->closeConnection();
}

void onConnection(std::shared_ptr<tinyNL::net::TcpConnection> con) {
    std::stringstream ss;
    ss << "usercode onCon new connection established. peer: " << tinyNL::net::AddressUtility::toString(con->peerAddr())
       << " port: "
       << ntohs(con->peerAddr().sin_port)
       <<std::endl;
    tinyNL::base::LOG<<ss.str();
}

void onPeerClose(std::shared_ptr<tinyNL::net::TcpConnection> con) {
    std::stringstream ss;
    ss<< "usercode onClose peer close :" << tinyNL::net::AddressUtility::toString(con->peerAddr())
      << " port: "
      << ntohs(con->peerAddr().sin_port)
      << std::endl;
    tinyNL::base::LOG<<ss.str();
}