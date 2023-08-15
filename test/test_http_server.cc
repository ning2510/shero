#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/coroutine/Hook.h"
#include "shero/net/http/HttpServer.h"
#include "shero/net/http/HttpServlet.h"

#include <memory>
#include <signal.h>

void Quit(int sig) {
    shero::EventLoop::GetEventLoop()->quit();
    exit(0);
}

class TestServlet : public shero::http::HttpServlet {
public:
    typedef std::shared_ptr<TestServlet> ptr;
    TestServlet(const std::string &name)
        : shero::http::HttpServlet(name) {
    }
    ~TestServlet() {}

    virtual void handle(shero::http::HttpRequest::ptr req, 
                shero::http::HttpResponse::ptr res) override {
        res->setBody("Test Servlet " + req->toString());
    }
};

int main() {
    signal(SIGINT, Quit);
    shero::EventLoop *loop = shero::EventLoop::GetEventLoop();
    std::cout << "Server tid = " << shero::GetThreadId() 
        << ", main loop = " << loop << std::endl;
    shero::Address address(9999);

    // set keep-alive false
    shero::http::HttpServer::ptr server(
            new shero::http::HttpServer(loop, address, "HttpServer", false));
    server->setThreadNums(8);

    shero::http::HttpDispatch *dispatch = server->getHttpDispatch();
    dispatch->addServlet("/shero/xxx", 
        [](shero::http::HttpRequest::ptr req, shero::http::HttpResponse::ptr res) {
            res->setBody("Shero Servlet " + req->toString());
        }
    );

    dispatch->addGlobServlet("/shero/*",
        [](shero::http::HttpRequest::ptr req, shero::http::HttpResponse::ptr res) {
            res->setBody("Shero Global Servlet " + req->toString());
        }
    );

    TestServlet::ptr testServlet(new TestServlet("TestServlet"));
    dispatch->addServlet("/shero/ccc", testServlet);

    server->start();

    return 0;
}