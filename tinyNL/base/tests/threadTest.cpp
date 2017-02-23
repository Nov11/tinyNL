//
// Created by c6s on 17-2-23.
//




#include <iostream>
#include <tinyNL/base/Thread.h>
#include <tinyNL/base/CountDownLatch.h>

using namespace tinyNL;
using namespace tinyNL::base;

CountDownLatch cdl(1);

void func(){
    std::cout<<"enter func"<<std::endl;
    cdl.await();
    std::cout<<"func return"<<std::endl;
}

void func1(){
    cdl.await();
    sleep(20);
    std::cout<<"func2 return"<<std::endl;
}
int main(){
    std::function<void()> function(::func);
    Thread thread(function);
    thread.run();

    Thread thread1(func1);
    thread1.run();

    sleep(2);
    cdl.countDown();
    thread.join();
//    thread1.join();
    std::cout<<"main return"<<std::endl;
}