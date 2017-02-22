//
// Created by c6s on 17-2-22.
//

#include <tinyNL/net/EventLoop.h>
#include <sys/timerfd.h>
#include <tinyNL/net/Channel.h>
#include <cstring>
#include <thread>
#include <tinyNL/net/Timer.h>
#include <tinyNL/base/TimeUtilies.h>

using namespace tinyNL;
using namespace tinyNL::net;
using namespace tinyNL::base;


std::atomic<EventLoop *> eventLoop;
std::shared_ptr<Timer> timer;

void oneShot() {
    std::cout << "one shot task" << std::endl;
}

void infiniteLoop(const std::string& str) {
    std::cout << "infiniteloop task " << str<< std::endl;
}

int cnt = 3;

void timerCallBack() {
    std::cout << "test timer call back" << std::endl;
    cnt--;
    if (cnt == 0) {
        eventLoop.load()->delTimer(timer);
    }
}

void *term(void *) {
    EventLoop local;
    eventLoop = &local;
    std::function<void()> cb(timerCallBack);
    timer = local.addTimerSinceNow(cb, 2 * 1000, 1 * 1000, 1);

    eventLoop.load()->addTimerSinceNow(oneShot, 1 * 1000, 0, 0);
//    std::function<void()> inif(infiniteLoop);
//    eventLoop.load()->addTimerSinceNow(inif, 1 * 1000, 1 * 1000, 1);
    local.loop();
    std::cout << "function term return" << std::endl;
    return nullptr;
}

class OneTask {
public:
    int a = 0;

    void update(int n) {
        a += n;
        std::cout << "OneTask update" << std::endl;
    }
};

int main() {

    std::thread th(term, nullptr);
//    pthread_t thread;
//    pthread_create(&thread, nullptr, term, nullptr);

    while (eventLoop.load() == nullptr);
    std::string one("one");
    std::string two("two");
    auto tmp = std::bind(&::infiniteLoop, one);
    std::function<void()> inf1 = tmp;
    auto tmp3 = std::bind(&::infiniteLoop, two);
    std::function<void()> inf2(tmp3);
    eventLoop.load()->addTimerSinceNow(inf1, 1 * 1000, 1 * 1000, 1);
    eventLoop.load()->addTimerSinceNow(inf2, 2 * 1000, 1 * 1000, 1);

    OneTask onetask;
    auto tmp2 = std::bind(&OneTask::update, &onetask, 10);
    std::function<void()> pendingTask(tmp2);
    eventLoop.load()->runInLoopThread(pendingTask);

    sleep(8);
    std::cout << "Main before calling stop" << std::endl;
    eventLoop.load()->stop();
    std::cout << "Main before calling join" << std::endl;
    th.join();
    std::cout << "Main OneTask a:" << onetask.a << std::endl;
    std::cout << "Main return" << std::endl;
}