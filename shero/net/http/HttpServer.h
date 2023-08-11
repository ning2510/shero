#ifndef __SHERO_HTTPSERVER_H
#define __SHERO_HTTPSERVER_H

#include "shero/base/Buffer.h"
#include "shero/net/Address.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpServer.h"
#include "shero/net/http/HttpDispatch.h"

#include <memory>

namespace shero {
namespace http {

class HttpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(EventLoop *loop, const Address &addr, 
            const std::string &name, bool keepAlive = false);
    ~HttpServer();

    void start();
    void stop();
    void setThreadNums(int32_t nums);

    bool isKeepAlive() const { return m_keepAlive; }
    EventLoop *getEventLoop() const { return m_loop; }
    HttpDispatch *getHttpDispatch() const { return m_dispatch.get(); }

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);

private:
    bool m_keepAlive;
    EventLoop *m_loop;
    TcpServer m_server;
    HttpDispatch::ptr m_dispatch;
};

}   // namespace http
}   // namespace shero

#endif
