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

void onMessageDiscard(const std::shared_ptr<TcpConnection>& con) {
    std::string msg = con->read();
    std::stringstream ss;
    ss << "discard :" << msg << " from " << AddressUtility::toString( con->peerAddr())
            << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();
}

void eventLoopThreadHandler(EventLoop* eventLoop){
    eventLoop->loop();
}

int main(){
    LOG<<"enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000)) ;
    //set discard callback
    srvptr->setOnMessageCallBack(::onMessageDiscard);
    //set worker thread
    EventLoopThread elt(::eventLoopThreadHandler);
    EventLoop* loop1 = elt.start();
    srvptr->addWorkEventLoop(loop1);
    srvptr->start();

    //timed shutdown tcp server after 60 sec from now.
    //first disable server, then stop main thread looping.
    loop.addTimerSinceNow([&loop,&srvptr](){srvptr->demolish();loop.stop();}, 60 * 1000, 0, 0);

    //main thread starts working here
    loop.loop();
    //stop event looping in worker thread
    loop1->stop();

    LOG<<"main return";
}