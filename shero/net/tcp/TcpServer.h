#ifndef __SHERO_TCPSERVER_H
#define __SHERO_TCPSERVER_H

#include "shero/net/Socket.h"
#include "shero/net/Address.h"
#include "shero/base/Noncopyable.h"
#include "shero/net/tcp/Callbacks.h"
#include "shero/net/tcp/TcpAcceptor.h"
#include "shero/net/tcp/TcpConnection.h"
#include "shero/net/EventLoopThreadPool.h"
#include "shero/coroutine/CoroutinePool.h"

#include <atomic>
#include <memory>
#include <functional>
#include <unordered_map>

namespace shero {

class EventLoop;

class TcpServer : public Noncopyable {
public:
    typedef std::shared_ptr<TcpServer> ptr;
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    TcpServer(EventLoop *mainLoop, 
        const Address &addr, const std::string &nameArg = "");
    ~TcpServer();

    void start();

    void setThreadNums(int32_t nums) { m_eventThreadPool->setThreadNums(nums); }

    void setThreadInitCallback(const ThreadInitCallback &cb) { m_threadInitCallback = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

private:
    void newConnection(int32_t connfd, Address peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

private:
    std::atomic_int m_start;
    int32_t m_nextConnId;
    std::string m_nameArg;

    EventLoop *m_mainLoop;
    CoroutinePool *m_coroutinePool;

    std::unique_ptr<TcpAcceptor> m_acceptor;
    EventLoopThreadPool::ptr m_eventThreadPool;
    
    ThreadInitCallback m_threadInitCallback;
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    std::unordered_map<int32_t, TcpConnection::ptr> m_connections;
};

}   // namespace shero

#endif
