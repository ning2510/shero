#include "shero/base/Log.h"
#include "shero/net/Socket.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"
// #include "shero/net/tcp/TcpClient.h"
#include "shero/net/tcp/TcpConnection.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

namespace shero {

TcpConnection::TcpConnection(int32_t connfd, EventLoop *subLoop, 
            const std::string &name, Address peerAddr)
    : m_stop(false),
      m_connfd(connfd),
      m_name(name),
      m_state(Connecting),
      m_channel(new Channel(subLoop, connfd)),
      m_input(),
      m_output(),
      m_peerAddr(peerAddr),
      m_subLoop(subLoop) {

    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {
    m_stop = true;
    LOG_INFO << "TcpConnection destructor [" << m_name 
        << "], connfd = " << m_connfd;
    assert(m_state == Disconnected);
}

void TcpConnection::connectEstablished() {
    m_subLoop->assertInLoopThread();
    assert(m_state == Connecting);
    setState(Connected);
    m_channel->tie(shared_from_this());
    m_channel->addListenEvents(IOEvent::READ);
    m_connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    m_subLoop->assertInLoopThread();
    if(m_state == Connected) {
        setState(Disconnected);
        m_channel->delAllListenEvents();
        m_connectionCallback(shared_from_this());
    }
    m_channel->removeFromLoop();
}

// send
void TcpConnection::send(const std::string &data) {
    if(m_state != Connected) {
        LOG_ERROR << "TcpConnection [" << m_name << "] has disconnected";
        return ;
    }

    if(m_subLoop->isInLoopThread()) {
        sendInLoop(data.c_str(), data.size());
    } else {
        std::shared_ptr<std::string> msg(new std::string(data));
        std::shared_ptr<size_t> len(new size_t(data.size()));

        m_subLoop->runInLoop(std::bind(
            &TcpConnection::sendInLoopPtr, this, msg, len));
    }
}

void TcpConnection::sendInLoopPtr(
        std::shared_ptr<std::string> data, std::shared_ptr<size_t> len) {
    sendInLoop(data->c_str(), *len);
}

void TcpConnection::sendInLoop(const void *data, size_t len) {
    m_subLoop->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool err = false;

    LOG_INFO << "send data = " << (const char *)data << ", len = " << len
        << " fd = " << m_channel->getFd();
    if(m_state == Disconnected) {
        LOG_ERROR << "TcpConnection [" << m_name 
            << "] is closed already, give up send data";
        return ;
    }

    // channel 没有注册 EPOLLOUT 事件并且缓冲区中没有待数据
    if(!m_channel->isWriting() && m_output.readableBytes() == 0) {
        nwrote = write(m_connfd, data, len);
        if(nwrote >= 0) {
            remaining = len - nwrote;
            if(remaining == 0 && m_writeCompleteCallback) {
                m_subLoop->queueInLoop(
                        std::bind(m_writeCompleteCallback, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if(errno != EWOULDBLOCK) {
                LOG_ERROR << "TcpConnection::sendInLoop error";

                if(errno == EPIPE || errno == ECONNRESET) {     // SIGPIPE  RESET
                    err = true;
                }
            }
        }
    }

    if(!err && remaining > 0) {
        m_output.writeLen((char *)data + nwrote, remaining);
        if(!m_channel->isWriting()) {
            m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
            m_channel->addListenEvents(IOEvent::WRITE);
        }
    }
}


// shutdown
void TcpConnection::shutdown() {
    if(m_state == Connected) {
        setState(Disconnecting);
        m_subLoop->runInLoop(
            std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    m_subLoop->assertInLoopThread();
    // 数据全部发送完毕
    if(!m_channel->isWriting()) {
        if(::shutdown(m_connfd, SHUT_WR) < 0) {
            LOG_ERROR << "::shutdown error";
        }
    }
}


// forceClose
void TcpConnection::forceClose() {
    if((m_state == Connected) || (m_state = Disconnecting)) {
        setState(Disconnecting);
        m_subLoop->queueInLoop(
            std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseInLoop() {
    m_subLoop->assertInLoopThread();
    if((m_state == Connected) || (m_state = Disconnecting)) {
        handleClose();
    }
}

// handle event
void TcpConnection::handleRead() {
    m_subLoop->assertInLoopThread();
    int32_t saveError = 0;
    size_t n = m_input.readFd(m_connfd, &saveError);
    
    if(n > 0) {
        m_messageCallback(shared_from_this(), &m_input);
    } else if(n == 0) {
        handleClose();
    } else {
        errno = saveError;
        LOG_ERROR << "TcpConnection::hanleRead error"
            << " errno = " << errno << " strerror = " << strerror(errno);
        handleError();
    }
}

void TcpConnection::handleWrite() {
    m_subLoop->assertInLoopThread();
    if(m_channel->isWriting()) {
        int32_t saveError;
        size_t n = m_output.writeFd(m_connfd, &saveError);
        if(n > 0) {
            m_output.retrieve(n);
            if(m_output.readableBytes() == 0) {
                m_channel->delListenEvents(IOEvent::WRITE);
                if(m_writeCompleteCallback) {
                    m_subLoop->queueInLoop(
                        std::bind(m_writeCompleteCallback, shared_from_this()));
                }
                if(m_state == Disconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            LOG_ERROR << "TcpConnection::handleWrite error"
                << " errno = " << errno << " strerror = " << strerror(errno);
        }
    } else {
        LOG_ERROR << "TcpConnection connfd = " << m_connfd
            << " is down, no more writing";
    }
}

void TcpConnection::handleClose() {
    m_subLoop->assertInLoopThread();
    LOG_INFO << "TcpConnection::handleClose connfd = " 
        << m_connfd << ", state = " << m_state;

    assert(m_state == Connected || m_state == Disconnecting);
    setState(Disconnected);
    m_channel->delAllListenEvents();

    TcpConnectionPtr guardConn(shared_from_this());
    m_connectionCallback(guardConn);
    m_closeCallback(guardConn);
}

void TcpConnection::handleError() {
    int32_t optval;
    socklen_t optlen = sizeof(optval);
    int32_t error = 0;
    if(::getsockopt(m_connfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        error = errno;
    } else {
        error = optval;
    }
    (void)error;
    LOG_ERROR << "TcpConnection::handleError name = " << m_name
        << " - SO_ERROR = " << error << ", strerror = " << strerror(error);
}

}   // namespace shero