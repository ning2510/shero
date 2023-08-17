#include "shero/base/Util.h"
#include "shero/net/Address.h"
// #include "shero/net/http/HttpCommon.h"
#include "shero/net/EventLoopThread.h"
#include "shero/net/http/client/HttpClient.h"

#include <sstream>
#include <iostream>

using namespace shero;
using namespace http;
using namespace client;

int main() {
    std::cout << "Client tid = " << GetThreadId() << std::endl;

    EventLoopThread loopThread;
    EventLoop *loop = loopThread.startLoop();

    // HttpResponse::ptr res = 
    //     HttpClient::DoGet(loop, "http://127.0.0.1:9999/shero/xxx", "HttpClient");

    HttpResponse::ptr res2 = 
        HttpClient::DoGet(loop, "http://www.baidu.com/", "HttpClient");

    // HttpResponse::ptr res3 = 
    //     HttpClient::DoGet(loop, "http://www.sylar.top/blog/", "HttpClient");

    return 0;
}