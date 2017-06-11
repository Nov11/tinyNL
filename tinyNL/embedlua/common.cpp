//
// Created by c6s on 17-6-2.
//

#include <cstdio>
#include <cstdlib>
#include "common.h"

#include <iostream>

using namespace std;

void error(lua_State *L, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    lua_close(L);
    exit(EXIT_FAILURE);
}

bool getNumber(lua_State *L, double &result, int index) {
    if (!lua_isnumber(L, index)) {
        cerr << "index :" << index << " is not a number :" << lua_tostring(L, -1) << endl;
        lua_pop(L, -1);
        return false;
    }
    result = lua_tonumber(L, index);
    lua_pop(L, 1);
    return true;
}