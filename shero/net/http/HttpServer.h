#ifndef __SHERO_HTTPSERVER_H
#define __SHERO_HTTPSERVER_H

#include "shero/net/tcp/TcpServer.h"
#include "shero/net/http/HttpCommon.h"
#include "shero/net/http/HttpDispatch.h"

namespace shero {
namespace http {

class HttpServer : public Noncopyable {
public:
    typedef std::function<void (const HttpRequest &req, HttpResponse *res)> HttpCallback;

    HttpServer(EventLoop *loop,
        const Address& listenAddr, const std::string& name);
    ~HttpServer() {}

    void start();

    EventLoop* getLoop() const { return m_server.getLoop(); }
    HttpDispatch *getHttpDispatch() const { return m_dispatch.get(); }
    void setHttpCallback(const HttpCallback& cb) { m_httpCallback = cb; }
    void setThreadNums(int numThreads) { m_server.setThreadNums(numThreads); }

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);
    void onRequest(const TcpConnectionPtr &conn, const HttpRequest &req);

    TcpServer m_server;
    HttpCallback m_httpCallback;
    HttpDispatch::ptr m_dispatch;
};

}   // namespace http
}   // namespace shero

#endif
