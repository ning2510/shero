#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/Address.h"
#include "shero/net/EventLoop.h"
#include "shero/coroutine/Hook.h"
#include "shero/net/tcp/TcpServer.h"

#include <signal.h>

using namespace shero;
using namespace std::placeholders;

void Quit(int sig) {
    EventLoop::GetEventLoop()->quit();
}

class EchoServer {
public:
    EchoServer(EventLoop *loop, Address::ptr addr, const std::string &name)
        : m_loop(loop),
          m_server(loop, addr, name) {
        
        m_server.setThreadNums(1);
        m_server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
        m_server.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2));
    }

    ~EchoServer() {}

    void start() {
        m_server.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn) {
        if(conn->isConnected()) {
            LOG_INFO << "Connection UP : " << conn->getPeerAddr()->toIpPort();
        } else {
            LOG_INFO << "Connection DOWN : " << conn->getPeerAddr()->toIpPort();
        }
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
        std::string msg = buf->retrieveAllAsString();
        msg = "EchoServer - " + msg;
        conn->send(msg);
    }

private:
    EventLoop *m_loop;
    TcpServer m_server;
};

int main() {
    set_hook_enable(false);
    signal(SIGINT, Quit);

    EventLoop *loop = EventLoop::GetEventLoop();
    std::cout << "Server tid = " << GetThreadId() 
        << ", main loop = " << loop << std::endl;
    
    Address::ptr addr(new Address(6666));
    
    EchoServer server(loop, addr, "EchoServer");
    server.start();
    loop->loop();

    std::cout << "main end\n";

    return 0;
}