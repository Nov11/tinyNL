//
// Created by c9s on 2017/5/22.
//

#include <iostream>

using namespace std;

#include <tinyNL/net/EventLoop.h>
#include <tinyNL/net/TcpServer.h>
#include <tinyNL/net/TcpConnection.h>
#include <tinyNL/net/AddressUtility.h>
#include <tinyNL/net/EventLoopThread.h>
#include <map>

#include "ls.h"

using namespace tinyNL::net;
using namespace tinyNL::base;

string test{"GET /rpc/obtainTicket.action?buildDate=20161122&buildNumber=2016.3.3+Build+CL-163.13906.4&clientVersion=4&hostName=c6s-ThinkPad-T420&machineId=dd6814d9-7869-4654-a607-9e31625e13d8&productCode=cfc7082d-ae43-4978-a2a2-46feb1679405&productFamilyId=cfc7082d-ae43-4978-a2a2-46feb1679405&salt=1495467406783&secure=false&userName=c6s&version=2016300&versionNumber=2016300 HTTP/1.1\r\nAccept-Encoding: gzip\r\nUser-Agent: Java/1.8.0_112-release\r\nHost: localhost:60000\r\nAccept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\r\nConnection: keep-alive\r\n\r\n"};

void onConnection(const std::shared_ptr<TcpConnection> &con) {
    std::stringstream ss;
    ss << "Time Server:" << " peer " << AddressUtility::toString(con->peerAddr())
       << " port:" << ntohs(con->peerAddr().sin_port)
       << std::endl;
    LOG << ss.str();

    //string convoies u32int in binary, which is 4 bytes.
    time_t t = ::time(nullptr);
    uint32_t u32 = static_cast<uint32_t>(t);
    std::string time(reinterpret_cast<char *>(&u32), sizeof(u32));
    LOG << std::to_string(u32);

}

struct HttpRequest {
    string line;
    vector<string> header;
    string body;
};
int status = 0;
string incomming;
HttpRequest req;
HttpRequest working;


void func(string input) {
    size_t crlf = input.find("\r\n");
    if (crlf == string::npos) {
        return;
    }
    while (!input.empty()) {
        switch (status) {
            case 0: {
                crlf += 2;
                req.line = input.substr(0, crlf);
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
                        working = req;
                        status = 0;
                        req.line.clear();
                        req.body.clear();
                        req.header.clear();
                        return;
                    }
                    //append headers to req

                    req.header.push_back(input.substr(0, p));
                    p += 2;
                    input = input.substr(p, input.size() - p);
                }

                break;
            }
        }//end switch
    }//end while
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
void split(string &s, vector<string> &result) {
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

static int rsa_signer(vector<unsigned char> &message, vector<unsigned char> &result) {
    rsa_context ctx;
    uint8_t hash[16];
    int ret;

    rsa_init(&ctx);
    mpi_init(&ctx.P);
    mpi_init(&ctx.Q);
    mpi_init(&ctx.N);
    mpi_init(&ctx.E);
    mpi_init(&ctx.D);
    mpi_read_string(&ctx.P, KEY_RADIX, KEY_P);
    mpi_read_string(&ctx.Q, KEY_RADIX, KEY_Q);
    mpi_read_string(&ctx.N, KEY_RADIX, KEY_N);
    mpi_read_string(&ctx.E, KEY_RADIX, KEY_E);
    mpi_read_string(&ctx.D, KEY_RADIX, KEY_D);
    ctx.len = KEY_LEN;

    md5(message.data(), message.size(), hash);
    result.resize(ctx.len);
    ret = rsa_sign(&ctx, hash, result.data());
    if (ret == 0) {

    } else {
        assert(false);
    }
    rsa_free(&ctx);
    return ret;
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
char hex(int i){
    if(i >= 0 && i <= 9){
        return static_cast<char>('0' + i);
    }
    return static_cast<char>(i - 10 + 'a');
}
vector<char> convertTo02X(unsigned char c){
    vector<char> result(2);
    int i = c;
    int least = i % 16;
    result[1] = hex(least);
    int high = i / 16;
    result[0] = hex(high);
    return result;
}
void onMsg(const std::shared_ptr<TcpConnection> &con) {
    string req_tmp = con->read();
    incomming.append(req_tmp);
    cout << incomming <<endl;
    //if incomming size exceeds a limit, drop all contents;
    if (incomming.size() > 2048) {
        incomming.clear();
        return;
    }
    string rest;
    size_t requestEnd = incomming.find("\r\n\r\n");
    if(requestEnd != string::npos){
        rest = incomming.substr(0, requestEnd + 4);

        incomming = incomming.substr(requestEnd + 4);
    }else{
        return;
    }
    func(rest);
//    if (!ready) { return; }


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
        split(params, urlParams);
        splitHeaders(urlParams, hash);
    }

    //what is the target method?
//    assert( == 0);
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

    vector<unsigned char> msg;
    for (size_t i = 0; i < response.size(); i++) {
        char c = response[i];
        unsigned char uc = c;
        assert(uc == c);
        msg.push_back(uc);
    }
    vector<unsigned char> result;
    rsa_signer(msg, result);
    string signedStr ;
    for(size_t i = 0; i < result.size(); i++){
        vector<char> vc = convertTo02X(result[i]);
        signedStr.push_back(vc[0]);
        signedStr.push_back(vc[1]);
    }

    string h = formatTo(PREHEADER, {signedStr, response});
    string finalMsg = composeResponse(h);
    con->send(finalMsg);
}


int main() {
    LOG << "enter main";

    EventLoop loop;
    std::shared_ptr<TcpServer> srvptr(new TcpServer(&loop, 60000));
    //set time callback
//    srvptr->setOnConnectionCallBack(::onConnection);
    srvptr->setOnMessageCallBack(::onMsg);
    srvptr->start();

    //timed shutdown tcp server after 60 sec from now.
    //first disable server, then stop main thread looping.
    loop.addTimerSinceNow([&loop, &srvptr]() {
        srvptr->demolish();
        loop.stop();
    }, 600 * 1000, 0, 0);

    //main thread starts working here
    loop.loop();

    LOG << "main return";
}
