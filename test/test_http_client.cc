#include "shero/base/Util.h"
#include "shero/net/Address.h"
#include "shero/net/http/Http.h"
#include "shero/coroutine/Hook.h"
#include "shero/net/http/HttpClient.h"
#include "shero/net/EventLoopThread.h"

#include <sstream>

int main() {
    std::cout << "Client tid = " << shero::GetThreadId() << std::endl;

    shero::EventLoopThread loopThread;
    shero::EventLoop *loop = loopThread.startLoop();

    // shero::http::HttpResponse::ptr res = 
    //     shero::http::HttpClient::DoGet(loop, "http://127.0.0.1:9999/shero/xxx", "HttpClient");

    shero::http::HttpResponse::ptr res2 = 
        shero::http::HttpClient::DoGet(loop, "http://www.baidu.com/", "HttpClient");

    // shero::http::HttpResponse::ptr res3 = 
    //     shero::http::HttpClient::DoGet(loop, "http://www.sylar.top/blog/", "HttpClient");

    return 0;
}