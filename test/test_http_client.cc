#include "shero/base/Util.h"
#include "shero/net/http/Http.h"
#include "shero/coroutine/Hook.h"
#include "shero/net/http/HttpClient.h"
#include "shero/net/EventLoopThread.h"

#include <sstream>

int main() {
    shero::set_hook_enable(false);
    std::cout << "Client tid = " << shero::GetThreadId() << std::endl;

    shero::EventLoopThread loopThread;
    shero::EventLoop *loop = loopThread.startLoop();

    // shero::http::HttpResponse::ptr res = 
    //     shero::http::HttpClient::DoGet(loop, "http://39.100.72.123/blog/", "HttpClient");

    shero::http::HttpResponse::ptr res = 
        shero::http::HttpClient::DoGet(loop, "http://220.181.38.150/", "HttpClient");

    return 0;
}