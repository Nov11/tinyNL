//
// Created by c6s on 17-2-25.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/Buffer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <thread>
#include <tinyNL/net/EventLoopThread.h>
#include <tinyNL/net/tests/commonccb.h>

using namespace tinyNL;
using namespace tinyNL::net;
EventLoop loop;

void shut(int delay){
    sleep(delay);
    loop.stop();
}

void pong(std::shared_ptr<TcpConnection> con){
    std::string receive = con->read();
    std::string output = "from peer:";
    output.append(receive);
    base::LOG <<output;
    if(receive.size() > 2){
        receive[1] = 'o';
    }
    con->send(receive);
}

int main(){
    std::thread th(::shut, 60);
    EventLoopThread elt1(infinitLoop);
    EventLoopThread elt2(infinitLoop);

    base::LOG<<"main thread";
    //basic setup
    std::shared_ptr<TcpServer> server(new TcpServer (&loop, 60000));
    server->setOnPeerCloseCallBack(::onPeerClose);
    server->setOnMessageCallBack(::pong);
    server->setOnConnectionCallBack(::onConnection);

    //setup working thread
    server->addWorkEventLoop(elt1.start());
    server->addWorkEventLoop(elt2.start());

    server->start();
    loop.loop();


    server->demolish();
    elt1.stop();
    elt2.stop();
    th.join();
    base::LOG<<"return from main\n";
}