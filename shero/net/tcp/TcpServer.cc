#include "shero/base/Log.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpServer.h"

#include <string.h>
#include <unistd.h>
#include <iostream>

namespace shero {

static EventLoop *CheckLoopIsNull(EventLoop *loop) {
    if(loop == nullptr) {
        LOG_FATAL << "main loop is nullptr";
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *mainLoop, 
    const Address &addr, const std::string &nameArg /*= ""*/)
    : m_start(0),
      m_nextConnId(1),
      m_nameArg(nameArg),
      m_mainLoop(mainLoop),
      m_coroutinePool(CoroutinePool::GetCoroutinePool()),
      m_acceptor(new TcpAcceptor(mainLoop, addr)),
      m_eventThreadPool(new EventLoopThreadPool(mainLoop, nameArg)),
      m_threadInitCallback(),
      m_connectionCallback(),
      m_messageCallback(),
      m_writeCompleteCallback() {
    
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection,
         this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
    LOG_INFO << "~TcpServer";
    for(auto &it : m_connections) {
        TcpConnectionPtr conn(it.second);
        conn->stop();
        it.second.reset();

        conn->getSubLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::start() {
    if(m_start++ == 0) {
        m_eventThreadPool->start(m_threadInitCallback);
        m_acceptor->listen();

        
        Coroutine::Resume(m_acceptor->getAcceptCor());
        m_mainLoop->loop();

        // m_mainLoop->runInLoop(
        //     std::bind(&TcpAcceptor::listen, m_acceptor.get()));
    }
}

void TcpServer::newConnection(int32_t connfd, Address peerAddr) {
    EventLoop *subLoop = m_eventThreadPool->GetNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%s#%d", 
        m_acceptor->getLocalAddr().toIpPort().c_str(), m_nextConnId);
    m_nextConnId++;

    std::string connName = m_nameArg + buf;
    LOG_INFO << "TcpServer::newConnection [" << connName 
        << "] created, connfd = " << connfd;
    
    TcpConnectionPtr conn(new TcpConnection(this, connfd, subLoop, connName, peerAddr));
    m_connections[connfd] = conn;

    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    subLoop->runInLoop(std::bind(&TcpConnection::ServerConnectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    m_mainLoop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << conn->getName() 
        << "] fd = " << conn->getConnfd();

    m_connections.erase(conn->getConnfd());
    EventLoop *subLoop = conn->getSubLoop();
    subLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}




}   // namespace shero