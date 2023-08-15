#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/EventLoop.h"
#include "shero/net/websocket/WSServer.h"
#include "shero/net/websocket/WSServlet.h"
#include "shero/net/websocket/WSDispatch.h"
#include "shero/net/websocket/WSStructure.h"

#include <memory>
#include <signal.h>

using namespace std;
using namespace shero;
using namespace ws;

void Quit(int sig) {
    shero::EventLoop::GetEventLoop()->quit();
    exit(0);
}

class WSTestServlet : public WSServlet {
public:
    typedef std::shared_ptr<WSTestServlet> ptr;
    WSTestServlet(const std::string &name)
        : WSServlet(name) {
    }
    ~WSTestServlet() {}

    virtual void handle(WSFrameMessage::ptr req, TcpConnectionPtr conn) override {
        req->setData("WSTestServlet " + req->getData());
        std::string data = WSFrameMessage::EncodeWSFrameMessage(req);
        conn->send(data);
    }

};

int main() {
    signal(SIGINT, Quit);
    EventLoop *loop = EventLoop::GetEventLoop();
    cout << "Server tid = " << GetThreadId() 
        << ", main loop = " << loop << endl;
    Address address(9999);

    WSServer::ptr server(new WSServer(loop, address, "WSServer"));
    WSDispatch *dispatch = server->getWSDispatch();
    dispatch->addServlet("/shero/xxx",
        [](WSFrameMessage::ptr req, TcpConnectionPtr conn) {
            req->setData("shero/xxx Servlet " + req->getData());
            std::string data = WSFrameMessage::EncodeWSFrameMessage(req);
            conn->send(data);
        }
    );

    dispatch->addGlobServlet("/shero/*",
        [](WSFrameMessage::ptr req, TcpConnectionPtr conn) {
            req->setData("shero/* Servlet " + req->getData());
            std::string data = WSFrameMessage::EncodeWSFrameMessage(req);
            conn->send(data);
        }
    );

    WSTestServlet::ptr testServlet(new WSTestServlet("WSTestServlet"));
    dispatch->addServlet("/shero/ccc", testServlet);

    server->start();

    return 0;
}