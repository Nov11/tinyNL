//
// Created by c9s on 2017/5/22.
//

#include <iostream>

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>
#include <map>

#include "ls.h"
#include "Search.h"

using namespace std;
using namespace tinyNL::net;
using namespace tinyNL::base;

//string test{"GET /rpc/obtainTicket.action?buildDate=20161122&buildNumber=2016.3.3+Build+CL-163.13906.4&clientVersion=4&hostName=c6s-ThinkPad-T420&machineId=dd6814d9-7869-4654-a607-9e31625e13d8&productCode=cfc7082d-ae43-4978-a2a2-46feb1679405&productFamilyId=cfc7082d-ae43-4978-a2a2-46feb1679405&salt=1495467406783&secure=false&userName=c6s&version=2016300&versionNumber=2016300 HTTP/1.1\r\nAccept-Encoding: gzip\r\nUser-Agent: Java/1.8.0_112-release\r\nHost: localhost:60000\r\nAccept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\r\nConnection: keep-alive\r\n\r\n"};
Search crnl("\r\n\r\n");

HttpRequest func(string input) {
    HttpRequest result;
    size_t crlf = input.find("\r\n");
    if (crlf == string::npos) {
        return result;
    }
    int status = 0;
    while (!input.empty()) {
        switch (status) {
            case 0: {
                crlf += 2;
                result.line = input.substr(0, crlf);
                input = input.substr(crlf, input.size() - crlf);
                status = 1;
                break;
            }
            case 1: {
                while (true) {
                    size_t p = input.find("\r\n");
                    if (p == string::npos) { break; }
                    if (p == 0) {
                        //this is the blank line after headers
                        return result;
                    }
                    //append headers to req

                    result.header.push_back(input.substr(0, p));
                    p += 2;
                    input = input.substr(p, input.size() - p);
                }

                break;
            }
        }//end switch
    }//end while
    return result;
}

//headers are in format : string=string, without trailing \r\n
void splitHeaders(vector<string> &headers, map<string, string> &hash) {
    if (headers.empty()) { return; }
    for (auto &item : headers) {
        size_t p = item.find("=");
        if (p == string::npos) { continue; }
        string key = item.substr(0, p);
        string value = item.substr(p + 1);
        hash[key] = value;
    }
}

//by '&'
void extractParams(string &s, vector<string> &result) {
    size_t j = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '&') {
            result.push_back(s.substr(j, i - j));
            j = i + 1;
        }
    }
    result.push_back(s.substr(j));
}

string formatTo(const string &format, const vector<string> &params) {
    stringstream ss;
    size_t j = 0;
    for (size_t i = 0; i < format.size(); i++) {
        if (format[i] == '%' && i + 1 < format.size() && format[i + 1] == 's') {
            i++;
            assert(j < params.size());
            ss << params[j];
            j++;
        } else {
            ss << format[i];
        }
    }
    return ss.str();
}

string composeResponse(string &response) {
    time_t timer;
    char buffer[256];
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 256, "%a, %d %b %Y %H:%M:%S GMT", tm_info);

    string reply(HTTP_OK);
    reply.append("Server: Jetty(9.2.10.v20150310)\r\n");
    reply.append("Date: ").append(buffer).append("\r\n");
    reply.append("Content-Type: ").append("application/xml").append("\r\n");
    reply.append("Content-Length: ").append(to_string(response.size())).append("\r\n\r\n");
    reply.append(response);
    return reply;
}

bool getHttpRequest(const std::shared_ptr<TcpConnection> &con, HttpRequest& result){
//    string req_tmp = con->read();
//    incomming.append(req_tmp);
//    cout << "request: " << incomming <<endl;
//    //if incomming size exceeds a limit, drop all contents;
//    if (incomming.size() > 4096) {
//        incomming.clear();
//        return;
//    }
//    string rest;
//    size_t requestEnd = incomming.find("\r\n\r\n");
//    if(requestEnd != string::npos){
//        rest = incomming.substr(0, requestEnd + 4);
//
//        incomming = incomming.substr(requestEnd + 4);
//    }else{
//        return;
//    }
//

    Buffer& readBuf = con->readBuf;
    char* ptr = readBuf.readPtr();
    size_t len = readBuf.readableSize();
    size_t pos = 0;
    if(crnl.match(ptr, len, &pos)) {
        string request(ptr, pos + 4);
        readBuf.erase(pos + 4);
        result = func(request);
        return true;
    }
    return false;
}

void onMsg(const std::shared_ptr<TcpConnection> &con) {
    HttpRequest working;
    if(!getHttpRequest(con, working)){ return;}

    //parse request command line
    vector<string> cmdLine;
    string &line = working.line;
    size_t p = line.find(" ");
    if (p == string::npos) { return; }
    cmdLine.push_back(line.substr(0, p));
    size_t tmp = p + 1;
    p = line.find(" ", tmp);
    if (p == string::npos) { return; }
    cmdLine.push_back(line.substr(tmp, p - tmp));
    cmdLine.push_back(line.substr(p + 1, line.size() - 2 - p - 1));

    //extract all the headers
    //extract headers in header field
    map<string, string> hash;
//    splitHeaders(working.header, hash);
    //extract params in url
    size_t param = cmdLine[1].find("?");
    if (param != string::npos) {
        string params = cmdLine[1].substr(param + 1);
        vector<string> urlParams;
        extractParams(params, urlParams);
        splitHeaders(urlParams, hash);
    }

    //what is the target method?
    if(cmdLine[1].find("/rpc/") != 0){return;}
    size_t dot = cmdLine[1].find(".action", 5);
    assert(dot != string::npos);
    string cmd = cmdLine[1].substr(5, dot - 5);

    //set up parameters
    string salt;
    if (hash.find(FIELD_SALT) != hash.end()) {
        salt = hash[FIELD_SALT];
    } else {
        salt = DEF_SALT;
    }
    string tick;

    if (hash.find(FIELD_TICK) != hash.end()) {
        tick = hash[FIELD_TICK];
    } else {
        tick = DEF_TICK;
    }

    string user = DEF_USER;
    if (hash.find(FIELD_USER) != hash.end()) {
        user = hash[FIELD_USER];
    }

    string prolong = to_string(DEF_PROLONG);
    //parameters are set

    string response;
    if (cmd == "obtainTicket") {
        response = formatTo(XML_TICKET, {prolong, salt, user});
    } else if (cmd == "ping") {
        response = formatTo(XML_PING, {salt});
    } else if (cmd == "prolongTicket") {
        response = formatTo(XML_PROLONG, {salt, tick});
    } else if (cmd == "releaseTicket") {
        response = formatTo(XML_RELEASE, {salt});
    } else {
        assert(false);
    }

    //sign it
    string signedStr = sign(response);
    string formated = formatTo(PREHEADER, {signedStr, response});
    string finalMsg = composeResponse(formated);
    con->send(finalMsg);
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
