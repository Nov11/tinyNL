//
// Created by c6s on 17-6-11.
//

//
// Created by c6s on 17-6-2.
//

//http://www.lua.org/pil/24.1.html
//load and run lua code from file

#include <lua5.2/lua.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void error (lua_State* L, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    lua_close(L);
    exit(EXIT_FAILURE);
}



int main(){
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);

    ifstream ifs("scripts/foo.lua");
    string line;
    while(getline(ifs, line)){
        bool ret = luaL_loadbuffer(L, line.data(), line.size(), "line") ||  lua_pcall(L, 0, 0, 0);
        if(ret){
            error(L, "%s", lua_tostring(L, -1));
        }
    }

    lua_close(L);
    return 0;
}

