// #include "shero/base/Log.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpAcceptor.h"

#include <unistd.h>

namespace shero {

TcpAcceptor::TcpAcceptor(EventLoop* loop, const Address& localAddr)
  : m_listenning(false),
    m_loop(loop),
    m_sock(new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP, true)),
    m_localAddr(localAddr),
    m_acceptChannel(loop, m_sock->getFd()) {

    m_sock->bind(&m_localAddr);
    m_acceptChannel.setReadCallback(std::bind(&TcpAcceptor::handleRead, this));
}

TcpAcceptor::~TcpAcceptor() {
    m_acceptChannel.delAllListenEvents();
    m_acceptChannel.removeFromLoop();
}

void TcpAcceptor::handleRead() {
    m_loop->assertInLoopThread();
    int32_t connfd = m_sock->accept(&m_peerAddr);
    if (connfd >= 0) {
        if (m_newConnectionCallback) {
            m_newConnectionCallback(connfd, m_peerAddr);
        } else {
            close(connfd);
        }
    } else {
        // LOG_ERROR << "accept err : " << errno;
        if(errno == EMFILE) {
            // LOG_ERROR << "sockfd reached limit";
        }
    }
}

void TcpAcceptor::listen() {
    m_loop->assertInLoopThread();
    m_listenning = true;
    bool rt = m_sock->listen();
    if(!rt) {
        // LOG_FATAL << "TcpAcceptor listen error";
    }
    m_acceptChannel.addListenEvents(IOEvent::READ);
}

}  // namespace shero