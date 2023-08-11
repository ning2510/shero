#ifndef __SHERO_TCPACCEPTOR_H
#define __SHERO_TCPACCEPTOR_H

#include "shero/net/Socket.h"
#include "shero/net/Address.h"
#include "shero/net/Channel.h"
#include "shero/coroutine/Coroutine.h"

#include <memory>
#include <functional>

namespace shero {

class EventLoop;

class TcpAcceptor {
public:
    typedef std::shared_ptr<TcpAcceptor> ptr;
    typedef std::function<void(int32_t connfd, Address addr)> NewConnectionCallback;

    TcpAcceptor(EventLoop *loop, const Address &localAddr);
    ~TcpAcceptor();

    void listen();

    void setNewConnectionCallback(const NewConnectionCallback &cb) {
        m_newConnectionCallback = std::move(cb);
    }

    int32_t getFd() const { return m_sock->getFd(); }
    Address getLocalAddr() const { return m_localAddr; }
    Address getPeerAddr() const { return m_peerAddr; }

    Coroutine *getAcceptCor() const { return m_acceptCor.get(); }

private:
    void MainLoopFunc();

private:
    bool m_stop;
    Socket::ptr m_sock;

    Address m_localAddr;
    Address m_peerAddr;
    Coroutine::ptr m_acceptCor;

    Channel::ptr m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
};

}   // namespace shero

#endif
