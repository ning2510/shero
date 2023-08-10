#ifndef __SHERO_TCPCONNECTOR_H
#define __SHERO_TCPCONNECTOR_H

#include "shero/base/Timer.h"
#include "shero/net/Address.h"
#include "shero/net/Channel.h"
#include "shero/net/tcp/Callbacks.h"

#include <memory>
#include <functional>

namespace shero {

class Channel;
class EventLoop;

class TcpConnector : public std::enable_shared_from_this<TcpConnector> {
public:
    typedef std::shared_ptr<TcpConnector> ptr;
    typedef std::function<void(int32_t sockfd)> NewConnectionCallback;

    TcpConnector(EventLoop *loop, Address::ptr serverAddr, bool canRetry = false);
    ~TcpConnector();

    void start();
    void restart();

    bool isConnected() const { return m_state == Connected; }
    Address::ptr getServerAddr() const { return m_serverAddr; }

    void setNewConnectionCallback(const NewConnectionCallback &cb) { m_newConnectionCallback = cb; }

private:
    enum ConnectorStatus {
        Disconnected,
        Connecting,
        Connected
    };

    void setState(ConnectorStatus state) { m_state = state; }

    void connect();
    void connecting(int32_t sockfd);

    void startInLoop();

    void retry(int32_t sockfd);
    void resetChannel();
    int32_t removeAndResetChannel();

    void handleWrite();
    void handleError();

private:
    static const int32_t InitRetryDelayMs;
    static const int32_t MaxRetryDelayMs;

    int32_t m_retryDelayMs;
    bool m_canRetry;
    bool m_connect;
    EventLoop *m_loop;
    Address::ptr m_serverAddr;
    Timer::ptr m_timer;
    ConnectorStatus m_state;
    Channel::ptr m_channel;

    NewConnectionCallback m_newConnectionCallback;
};

}   // namespace shero

#endif
