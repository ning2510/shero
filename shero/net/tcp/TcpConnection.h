#ifndef __SHERO_TCPCONNECTION_H
#define __SHERO_TCPCONNECTION_H

#include "shero/net/Channel.h"
#include "shero/net/Address.h"
#include "shero/net/Buffer.h"
#include "shero/base/Noncopyable.h"
#include "shero/net/tcp/Callbacks.h"
#include "shero/net/http/HttpParser.h"

#include <atomic>
#include <memory>

namespace shero {

class EventLoop;

class TcpConnection : public Noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<TcpConnection> ptr;
    TcpConnection(int32_t connfd, EventLoop *subLoop, 
            const std::string &name, Address peerAddr);
    ~TcpConnection();

    void send(const std::string &data);
    void shutdown();
    void forceClose();

    void connectEstablished();
    void connectDestroyed();

    int32_t getConnfd() const { return m_connfd; }
    EventLoop *getSubLoop() const { return m_subLoop; }
    const std::string &getName() const { return m_name; }
    const Address getPeerAddr() const { return m_peerAddr; }
    bool isConnected() const { return m_state == Connected; }

    bool isStop() const { return m_stop; }
    void stop() { m_stop = true; }


    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }


    void setContext(const http::HttpParser &context)
    { context_ = context; }

    const http::HttpParser &getContext() const
    { return context_; }

    http::HttpParser *getMutableContext()
    { return &context_; }

private:
    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };

    void setState(ConnectionState state) { m_state = state; }

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void* data, size_t len);
    void sendInLoopPtr(
        std::shared_ptr<std::string> data, std::shared_ptr<size_t> len);

    void shutdownInLoop();
    void forceCloseInLoop();

private:
    bool m_stop;
    int32_t m_connfd;
    std::string m_name;
    std::atomic_int m_state;

    std::unique_ptr<Channel> m_channel;

    Buffer m_input;
    Buffer m_output;
    Address m_peerAddr;
    EventLoop* m_subLoop;

    ConnectionCallback m_connectionCallback;
    CloseCallback m_closeCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    http::HttpParser context_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}  // namespace shero


#endif
