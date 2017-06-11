//
// Created by c6s on 17-6-11.
//
#include <iostream>

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>
#include <map>

using namespace std;
using namespace tinyNL::net;
using namespace tinyNL::base;


#include <lua5.2/lua.hpp>
#include <cmath>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include "common.h"
using namespace std;

static int l_sin (lua_State *L) {
    double d = luaL_checknumber(L, 1);  /* get argument */
    lua_pushnumber(L, sin(d));  /* push result */
    return 1;  /* number of results */
}

static int l_dir (lua_State *L) {
    DIR *dir;
    struct dirent *entry;
    int i;
    const char *path = luaL_checkstring(L, 1);

    /* open directory */
    dir = opendir(path);
    if (dir == NULL) {  /* error opening the directory? */
        lua_pushnil(L);  /* return nil and ... */
        lua_pushstring(L, strerror(errno));  /* error message */
        return 2;  /* number of results */
    }

    /* create result table */
    lua_newtable(L);
    i = 1;
    while ((entry = readdir(dir)) != NULL) {
        lua_pushnumber(L, i++);  /* push key */
        lua_pushstring(L, entry->d_name);  /* push value */
        lua_settable(L, -3);
    }

    closedir(dir);
    return 1;  /* table is already on top */
}

//well, I wanna ref connectionptr here
int send_in_lua(lua_State* L){
    const char* ptr = luaL_checkstring(L, 1);  /* get argument */

    string output(ptr);

    void* val = lua_touserdata(L, lua_upvalueindex(1));
//    cout << "get ptr from closure in send_in_lua con ptr:" << val << endl;
    const std::shared_ptr<TcpConnection> con = *reinterpret_cast<const std::shared_ptr<TcpConnection>*>(val);
    con->send(output);
    return 0;  /* number of results */
}


int callLua(const std::shared_ptr<TcpConnection> &con){
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, l_dir);
    lua_setglobal(L, "l_dir");
    //this is a short form of the two lines above
    lua_register(L, "l_sin", l_sin);

//    lua_register(L, "send", send_in_lua);

    //add ptr to con in stack
//    cout << "ptr to con : " << &con << endl;
    lua_pushlightuserdata(L, reinterpret_cast<void*>(const_cast<std::shared_ptr<TcpConnection>*>(&con)));
    lua_pushcclosure(L, &send_in_lua, 1);
    lua_setglobal(L, "send");

    //set this input string to global variable
    string in = con->read();
    cout << in << endl;
    lua_pushstring(L, in.c_str());
    lua_setglobal(L, "inputStr");

    if (luaL_loadfile(L, "scripts/calledinlua.lua") || lua_pcall(L, 0, 0, 0)) {
        error(L, "loading files:%s", lua_tostring(L, -1));
    }

    lua_close(L);
    return 0;
}



void onMsg(const std::shared_ptr<TcpConnection> &con) {
    callLua(con);
}


int main() {
    LOG << "enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000));
    //set time callback
    srvptr->setOnMessageCallBack(::onMsg);
    srvptr->start();

    //main thread starts working here
    loop.loop();

    LOG << "main return";
}
