//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/EventLoop.h>
#include <sys/timerfd.h>
#include <tinyNL/net/Channel.h>
#include <cstring>

using namespace tinyNL;
using namespace tinyNL::net;
EventLoop loop;
int repeat = 5;
int timerfd;

void r() {
    repeat--;
    std::cout << "enter read handler" << std::endl;
    int array[10] = {};
    ssize_t ret = read(timerfd, array, sizeof(array));
    std::cout << "return val: " << ret << std::endl;
    std::cout << "remaing call " << repeat << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
    if (repeat == 0) {
        std::cout << "end event loop" << std::endl;
        loop.stop();
    }
    std::cout << "read handler return" << std::endl;
}

class toy{
public:
    std::function<void()> f(){
        return [this](){read();};
    }

    void read(){
        std::cout<<"toy::read"<<std::endl;
        loop.stop();
    }
};


int main() {

    timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    Channel channel(timerfd, &loop);
    auto f = std::function<void()>(r);
    toy t;
    f = t.f();
    channel.setReadCallBack(f);
    channel.enableRead();
    struct itimerspec interval;
    ::memset(&interval, 0, sizeof(interval));

    interval.it_value.tv_sec = 1;
    interval.it_interval.tv_sec = 1;
    timerfd_settime(timerfd, 0, &interval, nullptr);
    loop.loop();

    std::cout << "test done" << std::endl;
}