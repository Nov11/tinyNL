//
// Created by c6s on 17-2-25.
//

#ifndef TINYNL_COMMONCCB_H
#define TINYNL_COMMONCCB_H

#include <memory>
#include <tinyNL/net/TcpConnection.h>

void onMessage(const std::shared_ptr<tinyNL::net::TcpConnection>& con);
void onConnection(const std::shared_ptr<tinyNL::net::TcpConnection>& con);
void onPeerClose(const std::shared_ptr<tinyNL::net::TcpConnection>& con);
void fail();
void suc(int);
void infinitLoop(tinyNL::net::EventLoop* loop_);
#endif //TINYNL_COMMONCCB_H
