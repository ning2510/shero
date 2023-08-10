#ifndef __SHERO_SOCKET_H
#define __SHERO_SOCKET_H

#include "shero/net/Address.h"
#include "shero/base/Noncopyable.h"

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>

namespace shero {

class Socket : public Noncopyable {
public:
    typedef std::shared_ptr<Socket> ptr;

    static Socket::ptr CreateTCP();
    static Socket::ptr CreateTCP(Address::ptr addr);
    static Socket::ptr CreateUDP();
    static Socket::ptr CreateUDP(Address::ptr addr);

    Socket(int32_t domain = AF_INET, int32_t type = SOCK_STREAM, 
        int32_t protocol = 0, bool nowCreate = false);
    ~Socket();

    bool init();
    void close();
    bool isInvalid();
    bool bind(Address *localAddr);
    bool bind(Address::ptr localAddr);
    bool listen(int32_t backlog = SOMAXCONN);
    int32_t accept(Address *peerAddr, bool setNonBlock = false);
    bool connect(Address::ptr peerAddr);

    Address::ptr getLocalAddr();
    Address::ptr getRemoteAddr();

    bool isConnected() const { return m_connected; }
    int32_t getFd() const { return m_fd; } 
    int32_t getDomain() const { return m_domain; }
    int32_t getType() const { return m_type; }
    int32_t getProtocol() const { return m_protocol; }

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);

    std::ostream &dump(std::ostream &os);

private:
    int32_t m_fd;
    int32_t m_domain;
    int32_t m_type;
    int32_t m_protocol;
    bool m_connected;
    bool m_created;

    Address::ptr m_localAddr;
    Address::ptr m_remoteAddr;
};

}   // namespace shero

#endif
