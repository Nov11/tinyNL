//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/EventLoop.h>
#include <sys/timerfd.h>
#include <tinyNL/net/Channel.h>
#include <cstring>
#include <tinyNL/base/TimeUtilies.h>

using namespace tinyNL;
using namespace tinyNL::net;
EventLoop loop;
int repeat = 5;
int timerfd;

void r() {
    repeat--;
    std::cout << "enter readFromSocket handler" << std::endl;
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
    std::cout << "readFromSocket handler return" << std::endl;
}

class toy{
public:
    std::function<void()> f(){
        return [this](){read();};
    }

    void read(){
        std::cout<<"toy::readFromSocket"<<std::endl;
        loop.stop();
    }
};


int main() {

    timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
    Channel channel(timerfd, &loop);
    auto f = std::function<void()>(r);
    toy t;
    f = t.f();
    channel.setReadCallBack(f);
    channel.enableRead();
    struct itimerspec interval;
    ::memset(&interval, 0, sizeof(interval));
    long ms = base::TimeUtilies::millionSecondsSinceEposh();
    std::cout << ms << std::endl;
    interval.it_value.tv_sec = ms / 1000 + 2;
    interval.it_interval.tv_sec = 1;
    timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &interval, nullptr);
    timerfd_gettime(timerfd, &interval);
    std::cout << "sec" << interval.it_value.tv_sec << " nsec" << interval.it_value.tv_nsec <<std::endl;
    loop.loop();

    std::cout << "test done" << std::endl;
}