#ifndef __SHERO_TCPACCEPTOR_H
#define __SHERO_TCPACCEPTOR_H

#include "shero/net/Socket.h"
#include "shero/net/Channel.h"
#include "shero/net/Address.h"

#include <memory>
#include <functional>

namespace shero {

class EventLoop;
class Address;

class TcpAcceptor : public Noncopyable {
public:
    typedef std::shared_ptr<TcpAcceptor> ptr;
    typedef std::function<void(int32_t connfd, Address addr)> NewConnectionCallback;

    TcpAcceptor(EventLoop *loop, const Address &localAddr);
    ~TcpAcceptor();

    void listen();
  
    void setNewConnectionCallback(const NewConnectionCallback &cb) {
        m_newConnectionCallback = cb;
    }

    bool listening() const { return m_listenning; }
    int32_t getFd() const { return m_sock->getFd(); }
    Address getLocalAddr() const { return m_localAddr; }
    Address getPeerAddr() const { return m_peerAddr; }

private:
    void handleRead();

private:
    bool m_listenning;
    EventLoop *m_loop;
    Socket::ptr m_sock;

    Address m_localAddr;
    Address m_peerAddr;

    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
};

}  // namespace shero

#endif
