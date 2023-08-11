#include "shero/base/Log.h"
#include "shero/net/Socket.h"

#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>

namespace shero {

Socket::ptr Socket::CreateTCP() {
    return std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
}

Socket::ptr Socket::CreateTCP(Address::ptr addr) {
    return std::make_shared<Socket>(addr->getFamily(), SOCK_STREAM, 0);
}

Socket::ptr Socket::CreateUDP() {
    return std::make_shared<Socket>(AF_INET, SOCK_DGRAM, 0);
}

Socket::ptr Socket::CreateUDP(Address::ptr addr) {
    return std::make_shared<Socket>(addr->getFamily(), SOCK_DGRAM, 0);
}

Socket::Socket(int32_t domain /*= AF_INET*/, 
    int32_t type /*= SOCK_STREAM*/, int32_t protocol /*= 0*/, bool nowCreate /*= false*/)
    : m_fd(-1),
      m_domain(domain),
      m_type(type),
      m_protocol(protocol),
      m_connected(false),
      m_created(false) {
    if(nowCreate) {
        init();
    }
}

Socket::~Socket() {
    close();
}

void Socket::close() {
    if(!isConnected() && m_fd == -1) {
        return ;
    }

    m_connected = false;
    if(m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool Socket::isInvalid() {
    return m_fd == -1;
}

bool Socket::init() {
    if(m_created) {
        return true;
    }
    m_fd = socket(m_domain, m_type, m_protocol);
    LOG_DEBUG << "Socket::init() socket fd = " << m_fd;
    if(m_fd < 0) {
        LOG_ERROR << "::socket() error, strerror = " << strerror(errno);
        return false;
    }
    setReuseAddr(true);
    if(m_type == SOCK_STREAM) {
        setTcpNoDelay(true);
    }
    m_created = true;
    return true;
}

bool Socket::bind(Address *localAddr) {
    if(isInvalid()) {
        LOG_ERROR << "Socket::bind() invalid socket";
        return false;
    }

    int32_t rt = ::bind(m_fd, localAddr->getAddr(), localAddr->getAddrLen());
    if(rt < 0) {
        LOG_ERROR << "::bind() error, strerror = " << strerror(errno);
    }
    return rt >= 0;
}

bool Socket::bind(Address::ptr localAddr) {
    return bind(localAddr.get());
}

bool Socket::listen(int32_t backlog /*= SOMAXCONN*/) {
    if(isInvalid()) {
        LOG_ERROR << "Socket::listen() invalid socket";
        return false;
    }

    int32_t rt = ::listen(m_fd, backlog);
    if(rt < 0) {
        LOG_ERROR << "::listen() error, strerror = " << strerror(errno);
    }
    return rt >= 0;
}

int32_t Socket::accept(Address *peerAddr, bool setNonBlock /*= false*/) {
    if(isInvalid()) {
        LOG_ERROR << "Socket::accept() invalid socket";
        return false;
    }

    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr, sizeof(addr));

    // set connfd nonblock
    int32_t connfd;
    if(setNonBlock) {
        connfd = ::accept4(m_fd, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    } else {
        connfd = ::accept(m_fd, (sockaddr *)&addr, &len);
    }
    if(connfd < 0) {
        LOG_ERROR << "::accept() error, errno = "
             << errno << " strerror = " << strerror(errno);
    } else {
        peerAddr->setAddr(addr);
    }
    return connfd;
}

bool Socket::connect(Address::ptr peerAddr) {
    if(isInvalid()) {
        LOG_ERROR << "Socket::connect() invalid socket";
        return false;
    }

    int32_t rt = ::connect(m_fd, peerAddr->getAddr(), peerAddr->getAddrLen());
    if(rt < 0) {
        LOG_ERROR << "::connect() error, strerror = " << strerror(errno);
    } else {
        m_connected = true;
    }
    return m_connected;
}

Address::ptr Socket::getLocalAddr() {
    if(m_localAddr) {
        return m_localAddr;
    }
    
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int32_t rt = getsockname(m_fd, (sockaddr *)&addr, &len);
    if(rt < 0) {
        LOG_ERROR << "getsockname() error, strerror = " << strerror(errno);
    }

    m_localAddr = std::make_shared<Address>(addr);
    return m_localAddr;
}

Address::ptr Socket::getRemoteAddr() {
    if(m_remoteAddr) {
        return m_remoteAddr;
    }

    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int32_t rt = getpeername(m_fd, (sockaddr *)&addr, &len);
    if(rt < 0) {
        LOG_ERROR << "getpeername() error, strerror = " << strerror(errno);
    }

    m_remoteAddr = std::make_shared<Address>(addr);
    return m_remoteAddr;
}

void Socket::setTcpNoDelay(bool on) {
    int32_t optval = on ? 1 : 0;
    setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on) {
    int32_t optval = on ? 1 : 0;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
    int32_t optval = on ? 1 : 0;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}


}   // namespace shero