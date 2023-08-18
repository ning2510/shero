#ifndef __SHERO_WSCLIENT_H
#define __SHERO_WSCLIENT_H

#include "shero/base/Mutex.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpClient.h"
#include "shero/net/tcp/TcpConnection.h"
#include "shero/net/websocket/WSStructure.h"
#include "shero/net/http/client/HttpClient.h"

#include <vector>
#include <memory>

namespace shero {
namespace ws {

class WSClient {
public:
    typedef std::shared_ptr<WSClient> ptr;
    typedef Mutex MutexType;
    typedef std::function<void(const WSFrameMessage::ptr &)> WSMessageCallback;
    typedef std::function<void()> WSConnectionCallback;

    WSClient(EventLoop *loop, const std::string &url,
        const std::string &name, bool retry = false);
    ~WSClient();

    void sendHttp(const std::string &msg);
    bool sendWS(const std::string& msg, uint32_t opcode, bool fin = true);
    void connect();
    void disconnect();

    void setWSMessageCallback(const WSMessageCallback &cb) { m_messageCallback = cb; }
    void setWSConnectionCallback(const WSConnectionCallback &cb) { m_connectionCallback = cb; }
    bool isConnected() { return m_connect && m_conn->isConnected(); }
    EventLoop *getEventLoop() const { return m_loop; }
    int32_t getState() { return m_state; }

private:
    enum WSState {
        NONE = 0,
        TCP,
        HTTP,
        WS
    };

    void send();
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);
    void WSHandShake();
    void setState(WSState state) { m_state = state; }

private:
    bool m_connect;
    EventLoop *m_loop;
    MutexType m_mutex;
    std::string m_key;
    WSMessageCallback m_messageCallback;
    WSConnectionCallback m_connectionCallback;

    WSState m_state;
    Uri::ptr m_uri;
    TcpClient m_client;
    TcpConnectionPtr m_conn;
    std::vector<WSFrameMessage::ptr> m_messages;
};

}   // namespace ws
}   // namespace shero

#endif
