#ifndef __SHERO_TCPACCEPTOR_H
#define __SHERO_TCPACCEPTOR_H

#include "shero/net/Socket.h"
#include "shero/net/Address.h"
#include "shero/net/Channel.h"

#include <memory>
#include <functional>

namespace shero {

class EventLoop;

class TcpAcceptor {
public:
    typedef std::shared_ptr<TcpAcceptor> ptr;
    typedef std::function<void(int32_t connfd, Address::ptr addr)> NewConnectionCallback;

    TcpAcceptor(EventLoop *loop, Address::ptr localAddr);
    ~TcpAcceptor();

    void listen();

    void setNewConnectionCallback(const NewConnectionCallback &cb) {
        m_newConnectionCallback = std::move(cb);
    }

    int32_t getFd() const { return m_sock->getFd(); }
    Address *getLocalAddr() const { return m_localAddr.get(); }
    Address *getPeerAddr() const { return m_peerAddr.get(); }

private:
    void handleRead();

private:
    Socket::ptr m_sock;

    Address::ptr m_localAddr;
    Address::ptr m_peerAddr;

    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
};

}   // namespace shero

#endif
