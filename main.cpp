#include <iostream>
#include <tinyNL/net/CurrentThread.h>
int main() {
    std::cout << "Hello, World!" << std::endl;
    pid_t ret = tinyNL::net::CurrentThread::tid();
    std::cout << ret << std::endl;
    return 0;
}