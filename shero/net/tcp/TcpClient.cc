#include "shero/base/Log.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpClient.h"

#include <string.h>

namespace shero {

void defaultRemoveConnection(EventLoop *loop, const TcpConnectionPtr &conn) {
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void defaultConnectCallback(const TcpConnectionPtr &conn) {
    LOG_INFO << "connect to " << conn->getPeerAddr().toIpPort() << " is " << (conn->isConnected() ? "UP" : "DOWN");
}

void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buf) {
    std::string msg = buf->retrieveAllAsString();
    LOG_INFO << "recv message = " << msg;
}

TcpClient::TcpClient(EventLoop *loop, Address::ptr serverAddr, 
    bool retry /*= false*/, const std::string &nameArg /*= ""*/)
    : m_connect(false),
      m_retry(retry),
      m_nextConnId(1),
      m_nameArg(nameArg),
      m_loop(loop),
      m_connector(new TcpConnector(loop, serverAddr)),
      m_connectionCallback(),
      m_messageCallback(),
      m_writeCompleteCallback() {
   
    m_connector->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient() {
    LOG_INFO << "TcpClient::~TcpClient [" << m_nameArg << "]";

    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexType::Lock lock(m_mutex);
        unique = m_conn.unique();
        conn = m_conn;
    }

    if(conn) {
        CloseCallback cb = std::bind(&defaultRemoveConnection, m_loop, std::placeholders::_1);
        m_loop->runInLoop(
                std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if(unique) {
            conn->forceClose();
        }
    }
}


void TcpClient::connect() {
    LOG_INFO << "TcpClient::connect [" << m_nameArg << "] - connecting to " 
        << m_connector->getServerAddr()->toIpPort();

    m_connect = true;
    m_connector->start();
}

void TcpClient::disconnect() {
    m_connect = false;
    {
        MutexType::Lock lock(m_mutex);
        if(m_conn) {
            LOG_INFO << "TcpClient::disconnect TcpConnection[" << m_conn->getName() << "]";
            m_conn->shutdown();
        }
    }
}

void TcpClient::newConnection(int32_t sockfd) {
    LOG_DEBUG << "TcpClient::newConnection sockfd = " << sockfd;
    m_loop->assertInLoopThread();

    struct sockaddr_in peer;
    bzero(&peer, sizeof(peer));
    socklen_t len = sizeof(peer);
    if(::getpeername(sockfd, (sockaddr *)&peer, &len) < 0) {
        LOG_ERROR << "getpeername error, errno = " << errno
            << ", strerror = " << strerror(errno); 
    }
    Address::ptr peerAddr(new Address(peer));
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr->toIpPort().c_str(), m_nextConnId);
    m_nextConnId++;
    std::string connName = m_nameArg + buf;

    Address addr = *peerAddr;
    TcpConnectionPtr conn(new TcpConnection(this, sockfd, m_loop, connName, addr));
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(
        std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

    {
        MutexType::Lock lock(m_mutex);
        m_conn = conn;
    }

    conn->ClientconnectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr &conn) {
    m_loop->assertInLoopThread();
    {
        MutexType::Lock lock(m_mutex);
        m_conn.reset();
    }

    m_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if(m_retry && m_connect) {
        LOG_INFO << "TcpClient::removeConnection [" << conn->getName()
            << "] reconnecting to " << m_connector->getServerAddr()->toIpPort();
        m_connector->restart();
    }
}


}   // namespace shero