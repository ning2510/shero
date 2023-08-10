#include "shero/base/Log.h"
#include "shero/net/tcp/TcpAcceptor.h"
#include "shero/coroutine/CoroutinePool.h"

#include <unistd.h>

namespace shero {

TcpAcceptor::TcpAcceptor(EventLoop *loop, const Address &localAddr)
    : m_stop(false),
      m_sock(new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, true)),
      m_localAddr(localAddr)
    //   ,
    //   m_peerAddr(new Address())
       {
    bool rt = m_sock->init();
    if(!rt) {
        return ;
    }

    rt = m_sock->bind(&m_localAddr);
    if(!rt) {
        return ;
    }
    
    m_acceptChannel = ChannelMgr::GetInstance()->getChannel(m_sock->getFd(), loop);
    // m_acceptChannel.setReadCallback(std::bind(&TcpAcceptor::handleRead, this));
    m_acceptCor = CoroutinePool::GetCoroutinePool()->getCoroutineInstance();
    m_acceptCor->setCallback(std::bind(&TcpAcceptor::MainLoopFunc, this));
}

TcpAcceptor::~TcpAcceptor() {
    m_stop = true;
    CoroutinePool::GetCoroutinePool()->releaseCoroutine(m_acceptCor);
    m_acceptChannel->delAllListenEvents();
    m_acceptChannel->removeFromLoop();
}

void TcpAcceptor::MainLoopFunc() {
    while(!m_stop) {
        int32_t connfd = m_sock->accept(&m_peerAddr);
        if(connfd == -1) {
            LOG_ERROR << "TcpAcceptor accept error, Yield";
            Coroutine::Yield();
            continue;
        }

        if(m_newConnectionCallback) {
            m_newConnectionCallback(connfd, m_peerAddr);
        } else {
            close(connfd);
        }
    }
}

void TcpAcceptor::listen() {
    bool rt = m_sock->listen();
    if(!rt) {
        LOG_FATAL << "TcpAcceptor listen error";
    }

    // m_acceptChannel.addListenEvents(IOEvent::READ);
}

void TcpAcceptor::handleRead() {
    int32_t connfd = m_sock->accept(&m_peerAddr);
    if(connfd > 0) {
        if(m_newConnectionCallback) {
            m_newConnectionCallback(connfd, m_peerAddr);
        } else {
            close(connfd);
        }
    }
}

}   // namespace shero