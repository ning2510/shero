#ifndef __SHERO_TCPCONNECTION_H
#define __SHERO_TCPCONNECTION_H

#include "shero/net/Address.h"
#include "shero/base/Buffer.h"
#include "shero/base/Noncopyable.h"
#include "shero/net/tcp/Callbacks.h"

#include <atomic>
#include <memory>

namespace shero {

class TcpServer;
class EventLoop;

class TcpConnection : public Noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<TcpConnection> ptr;
    TcpConnection(int32_t connfd, EventLoop *subLoop, 
            const std::string &name, Address::ptr peerAddr);
    ~TcpConnection();

    void send(const std::string &data);
    void shutdown();
    void forceClose();

    void connectEstablished();
    void connectDestroyed();

    int32_t getConnfd() const { return m_connfd; }
    EventLoop *getSubLoop() const { return m_subLoop; }
    const std::string &getName() const { return m_name; }
    const Address::ptr getPeerAddr() const { return m_peerAddr; }
    bool isConnected() const { return m_state == Connected; }

    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

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

    void sendInLoop(const void *data, size_t len);
    void sendInLoopPtr(std::shared_ptr<std::string> data, std::shared_ptr<size_t> len);
    
    void shutdownInLoop();
    void forceCloseInLoop();

private:
    int32_t m_connfd;
    std::string m_name;
    std::atomic_int m_state;

    std::unique_ptr<Channel> m_channel;

    Buffer m_input;
    Buffer m_output;
    Address::ptr m_peerAddr;
    EventLoop *m_subLoop;

    ConnectionCallback m_connectionCallback;
    CloseCallback m_closeCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
};

}   // namespace shero

#endif
