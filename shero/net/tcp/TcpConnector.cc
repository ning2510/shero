#include "shero/base/Log.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpConnector.h"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace shero {

const int32_t TcpConnector::InitRetryDelayMs = 500;
const int32_t TcpConnector::MaxRetryDelayMs = 30 * 1000;

static int32_t createNonBlocking() {
    int32_t sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(sockfd < 0) {
        LOG_FATAL << "createNonBlocking error, errno = " << errno
            << ", strerror = " << strerror(errno);
    } else {
        LOG_INFO << "TcpConnector createNonBlocking success, sockfd = " << sockfd;
    }
    return sockfd;
}

TcpConnector::TcpConnector(EventLoop *loop, Address::ptr serverAddr, bool canRetry /*= false*/)
    : m_retryDelayMs(InitRetryDelayMs),
      m_canRetry(canRetry),
      m_connect(false),
      m_loop(loop),
      m_serverAddr(serverAddr),
      m_timer(new Timer(loop)),
      m_state(Disconnected),
      m_newConnectionCallback() {
}

TcpConnector::~TcpConnector() {
}


void TcpConnector::start() {
    m_connect = true;
    m_loop->runInLoop(std::bind(&TcpConnector::startInLoop, this));
}

void TcpConnector::startInLoop() {
    m_loop->assertInLoopThread();
    if(m_state == Disconnected) {
        if(m_connect) {
            connect();
        }
    }
}

void TcpConnector::restart() {
    m_loop->assertInLoopThread();
    setState(Disconnected);
    m_connect = true;
    m_retryDelayMs = InitRetryDelayMs;
    startInLoop();
}

void TcpConnector::retry(int32_t sockfd) {
    ::close(sockfd);
    setState(Disconnected);

    if(!m_canRetry) {
        return ;
    }

    if(m_connect) {
        LOG_INFO << "TcpConnector::retry connecting to "
            << m_serverAddr->toIpPort() << " in " << m_retryDelayMs << " milliseconds";

        auto it = shared_from_this();
        m_timer->addTimer(m_retryDelayMs, 
            std::bind(&TcpConnector::startInLoop, shared_from_this()), false);
        m_retryDelayMs = std::min(m_retryDelayMs * 2, MaxRetryDelayMs);
    } else {
        LOG_INFO << "TcpConnector::retry failed, m_connect is false";
    }
}

void TcpConnector::resetChannel() {
    m_channel.reset();
}

int32_t TcpConnector::removeAndResetChannel() {
    m_channel->delAllListenEvents();
    m_channel->removeFromLoop();
    int32_t sockfd = m_channel->getFd();

    m_loop->queueInLoop(std::bind(&TcpConnector::resetChannel, this));
    return sockfd;
}

void TcpConnector::connect() {
    int32_t sockfd = createNonBlocking();
    socklen_t len = sizeof(sockaddr_in);
    int32_t ret = ::connect(sockfd, m_serverAddr->getAddr(), len);
    int32_t savedErrno = (ret == 0) ? 0 : errno;
    switch(savedErrno) {
        case 0:
        case EINPROGRESS:   // 套接字为非阻塞套接字，且连接请求没有立即完成
        case EINTR:         // 系统调用的执行由于捕获中断而中止
        case EISCONN:       // 已经连接到该套接字
        connecting(sockfd);
        break;

        case EAGAIN:        // 没有足够空闲的本地端口
        case EADDRINUSE:    // 本地地址处于使用状态
        case EADDRNOTAVAIL: // 无法分配请求的地址
        case ECONNREFUSED:  // 远程地址并没有处于监听状态
        case ENETUNREACH:   // 网络不可达
        retry(sockfd);
        break;

        default:
            LOG_ERROR << "Unexpected error in Connector::startInLoop " << savedErrno;
            ::close(sockfd);
            break;
    }
}

void TcpConnector::connecting(int32_t sockfd) {
    setState(Connecting);
    m_channel.reset(new Channel(m_loop, sockfd));
    m_channel->setWriteCallback(std::bind(&TcpConnector::handleWrite, this));
    m_channel->setErrorCallback(std::bind(&TcpConnector::handleError, this));

    m_channel->addListenEvents(IOEvent::WRITE);
}

void TcpConnector::handleWrite() {
    if(m_state == Connecting) {
        int32_t sockfd = removeAndResetChannel();
        int32_t err, optval;
        socklen_t len = sizeof(optval);
        if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &len) < 0) {
            err = errno;
        } else {
            err = optval;
        }

        if(err) {
            retry(sockfd);
        } else {
            setState(Connected);
            if(m_connect) {
                m_newConnectionCallback(sockfd);
            } else {
                ::close(sockfd);
            }
        }
    }
}

void TcpConnector::handleError() {
    LOG_ERROR << "TcpConnector::handleError() state = " << m_state;
    if(m_state == Connecting) {
        int32_t sockfd = removeAndResetChannel();
        int32_t err, optval;
        socklen_t len = sizeof(optval);
        if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &len) < 0) {
            err = errno;
        } else {
            err = optval;
        }
        LOG_ERROR << "TcpConnector::handleError(), errno = " << err
            << ", strerror = " << strerror(err);
        retry(sockfd);
    }
}


}   // namespace shero