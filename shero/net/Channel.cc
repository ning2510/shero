// #include "shero/base/Log.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"

#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <poll.h>

namespace shero {

// Channel
Channel::Channel(EventLoop *loop, int32_t fd)
    : m_fd(fd),
      m_event(IOEvent::NONE),
      m_revents(IOEvent::NONE),
      m_loop(loop),
      m_status(ChannelStatus::NEW),
      m_tied(false) {
    // LOG_INFO << "new Channel created, loop = " << loop << ", fd = " << fd;
}

Channel::~Channel() {
}

void Channel::addListenEvents(IOEvent event) {
    if(event == IOEvent::READ) {
        m_event |= IOEvent::READ;
    } else if(event == IOEvent::WRITE) {
        m_event |= IOEvent::WRITE;
    } else {
        // LOG_WARN << "Channel::addListenEvents() IOEvent is invalid";
        return ;
    }
    updateToLoop();
}

void Channel::delListenEvents(IOEvent event) {
    if(event == IOEvent::READ) {
        m_event &= ~IOEvent::READ;
    } else if(event == IOEvent::WRITE) {
        m_event &= ~IOEvent::WRITE;
    } else {
        // LOG_WARN << "Channel::delListenEvents() IOEvent is invalid";
        return ;
    }
    updateToLoop();
}

void Channel::delAllListenEvents() {
  m_event = IOEvent::NONE; 
  updateToLoop();
}

void Channel::handleEvent() {
    if(m_tied) {
        auto it = m_tie.lock();
        if(it) {
            handleEventWithGuard();
        }
    } else {
        handleEventWithGuard();
    }
}

void Channel::tie(const std::shared_ptr<void> &v) {
    if(v) {
        m_tie = v;
        m_tied = true;
    }
}

void Channel::setNonBlock() {
    int32_t flag = fcntl(m_fd, F_GETFL, 0);
    if(flag & O_NONBLOCK) {
        return ;
    }

    fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    flag = fcntl(m_fd, F_GETFL, 0);
    if(flag & O_NONBLOCK) {
        // LOG_INFO << "set nonblock success, fd = " << m_fd;
    } else {
        // LOG_ERROR << "set nonblock failed, fd = " << m_fd
        //     << " strerror = " << strerror(errno);
    }
}

void Channel::removeFromLoop() {
    m_loop->removeChannel(this);
}

void Channel::updateToLoop() {
    m_loop->updateChannel(this);
}

void Channel::handleEventWithGuard() {
    if((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN)) {
        if(m_closeCallback) {
            m_closeCallback();
        }
    }
    if(m_revents & EPOLLERR) {
        if(m_errorCallback) {
            m_errorCallback();
        }
    }
    if(m_revents & (EPOLLIN | EPOLLPRI)) {
        if(m_readCallback) {
            m_readCallback();
        }
    }
    if(m_revents & EPOLLOUT) {
        if(m_writeCallback) {
            m_writeCallback();
        }
    }
}

}   // namespace shero