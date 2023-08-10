#include "shero/base/Log.h"
#include "shero/net/tcp/TcpAcceptor.h"

#include <unistd.h>

namespace shero {

TcpAcceptor::TcpAcceptor(EventLoop *loop, Address::ptr localAddr)
    : m_sock(new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, true)),
      m_localAddr(localAddr),
      m_peerAddr(new Address),
      m_acceptChannel(loop, m_sock->getFd()) {
    bool rt = m_sock->init();
    if(!rt) {
        return ;
    }

    rt = m_sock->bind(m_localAddr);
    if(!rt) {
        return ;
    }
    
    m_acceptChannel.setReadCallback(std::bind(&TcpAcceptor::handleRead, this));
}

TcpAcceptor::~TcpAcceptor() {
    m_acceptChannel.delAllListenEvents();
    m_acceptChannel.removeFromLoop();
}

void TcpAcceptor::listen() {
    bool rt = m_sock->listen();
    if(!rt) {
        LOG_FATAL << "TcpAcceptor listen error";
    }

    m_acceptChannel.addListenEvents(IOEvent::READ);
}

void TcpAcceptor::handleRead() {
    int32_t connfd = m_sock->accept(m_peerAddr);
    if(connfd > 0) {
        if(m_newConnectionCallback) {
            m_newConnectionCallback(connfd, m_peerAddr);
        } else {
            close(connfd);
        }
    }
}

}   // namespace shero