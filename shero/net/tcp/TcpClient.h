#ifndef __SHERO_TCPCLIENT_H
#define __SHERO_TCPCLIENT_H

#include "shero/base/Mutex.h"
#include "shero/net/Address.h"
#include "shero/net/tcp/Callbacks.h"
#include "shero/net/tcp/TcpConnector.h"
#include "shero/net/tcp/TcpConnection.h"

#include <atomic>
#include <memory>

namespace shero {

class EventLoop;

class TcpClient {
public:
    typedef std::shared_ptr<TcpClient> ptr;
    typedef Mutex MutexType;
    TcpClient(EventLoop *loop, Address::ptr serverAddr, 
        bool retry = false, const std::string &name = "");
    ~TcpClient();

    void connect();
    void disconnect();

    bool isConnected() { return m_conn && m_conn->isConnected(); }

    bool getRetry() const { return m_retry; } 
    void setRetry(bool v) { m_retry = v; }

    EventLoop *getMainLoop() const { return m_loop; }
    const std::string getName() const { return m_nameArg; }
    Address::ptr getServerAddr() const { return m_connector->getServerAddr(); }

    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

private:
    void newConnection(int32_t sockfd);
    void removeConnection(const TcpConnectionPtr &conn);

private:
    bool m_connect;
    bool m_retry;
    int32_t m_nextConnId;
    std::string m_nameArg;
    EventLoop *m_loop;
    MutexType m_mutex;
    
    TcpConnectionPtr m_conn;
    TcpConnector::ptr m_connector;
    
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
};

}   // namespace shero

#endif
