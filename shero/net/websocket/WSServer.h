#ifndef __SHERO_WSSERVER_H
#define __SHERO_WSSERVER_H

#include "shero/net/Buffer.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpServer.h"
#include "shero/net/http/HttpParser.h"
#include "shero/net/http/HttpServer.h"
#include "shero/net/websocket/WSDispatch.h"

#include <memory>
#include <unordered_map>

namespace shero {
namespace ws {

class WSServer {
public:
    typedef std::shared_ptr<WSServer> ptr;
    WSServer(EventLoop *loop, const Address &addr, const std::string &name);
    ~WSServer();

    void start();
    void stop();
    void setThreadNums(int32_t nums);

    bool isStop() const { return m_stop; }
    EventLoop *getEventLoop() const { return m_loop; }
    WSDispatch *getWSDispatch() const { return m_dispatch.get(); }

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);

    void HttpHandShake(const TcpConnectionPtr &conn, const http::HttpRequest &req);
    void WSCommunication(const TcpConnectionPtr &conn, Buffer *buf);

private:
    enum state {
        TCP = 1,
        WEBSOCKET = 2
    };

    bool m_stop;
    EventLoop *m_loop;
    TcpServer m_server;
    WSDispatch::ptr m_dispatch;
    std::unordered_map<TcpConnection *, std::pair<state, std::string> > m_conns;
};

}   // namespace ws
}   // namespace shero0

#endif
