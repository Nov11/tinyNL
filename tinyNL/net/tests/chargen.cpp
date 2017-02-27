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

std::string charseq = "~!@#$%^&*()_+`1234567890-=abcdefghijklmnopqrstuvwxyz\\][';/.,|}{\":?><:";
void onConnection(const std::shared_ptr<TcpConnection>& con) {
    std::stringstream ss;
    ss << "new connection from " << AddressUtility::toString( con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();
}

void onWriteComplete(const std::shared_ptr<TcpConnection>& con){
    con->send(charseq);
}

int main(){
    LOG<<"enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000)) ;
    srvptr->setOnConnectionCallBack(::onConnection);
    srvptr->setOnWriteComplete(::onWriteComplete);
    //set worker thread
    EventLoopThread elt;
    EventLoop* loop1 = elt.start();
    srvptr->addWorkEventLoop(loop1);
    srvptr->start();

    //timed shutdown tcp server after 60 sec from now.
    //first disable server, then stop main thread looping.
    loop.addTimerSinceNow([&loop,&srvptr](){srvptr->demolish();loop.stop();}, 60 * 1000, 0, 0);

    //main thread starts working here
    loop.loop();

    LOG<<"main return";
}