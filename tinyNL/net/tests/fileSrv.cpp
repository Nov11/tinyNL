//
// Created by c6s on 17-3-1.
//

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/EventLoopThread.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <fcntl.h>
#include <fstream>

using namespace tinyNL::base;
using namespace tinyNL::net;

std::string fileContent;

void onConnectionEstablished(const std::shared_ptr<TcpConnection> &con) {
    con->send(fileContent);
    con->closeConnectionOnWriteBuffDrained();
}

int main() {
    {
        char *home = getenv("HOME");
        std::string filePath(home);
        filePath.append("/randomFile");
        std::ifstream ifs(filePath.c_str(), std::ifstream::binary);
        std::ostringstream os;
        os << ifs.rdbuf();
        fileContent = os.str();
        std::cout << fileContent.size() << std::endl;
    }

    EventLoop eventLoop;
    EventLoopThread thread1;
    EventLoopThread thread2;

    std::shared_ptr<TcpServer> sptr(new TcpServer(&eventLoop, 60000));
    sptr->setOnConnectionCallBack(::onConnectionEstablished);
    sptr->addWorkEventLoop(thread1.start());
    sptr->addWorkEventLoop(thread2.start());
    sptr->start();

    eventLoop.addTimerSinceNow([&]() { eventLoop.stop(); }, 60 * 1000, 0, 0);
    eventLoop.loop();
    sptr->demolish();
    LOG << "main return";
}