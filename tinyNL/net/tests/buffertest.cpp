//
// Created by c6s on 17-2-23.
//

#include <string>
#include <tinyNL/net/Buffer.h>
#include <iostream>

using namespace tinyNL;
using namespace tinyNL::net;

int main(){
    Buffer b;
    std::string s("12345678910");
    b.append(s.data(), 10);
    b.erase(3);
    std::string tmp(b.readPtr(), b.readableSize());
    std::cout<< tmp<<std::endl;
}