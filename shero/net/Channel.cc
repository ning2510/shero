#include "shero/base/Log.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

namespace shero {

// Channel
Channel::Channel(EventLoop *loop, int32_t fd)
    : m_fd(fd),
      m_event(IOEvent::NONE),
      m_revents(IOEvent::NONE),
      m_loop(loop),
      m_cor(nullptr),
      m_status(ChannelStatus::NEW),
      m_tied(false) {
    LOG_INFO << "new Channel created, loop = " << loop << ", fd = " << fd;
}

Channel::~Channel() {
}

void Channel::addListenEvents(IOEvent event) {
    if(event == IOEvent::READ) {
        m_event |= IOEvent::READ;
    } else if(event == IOEvent::WRITE) {
        m_event |= IOEvent::WRITE;
    } else {
        LOG_WARN << "Channel::addListenEvents() IOEvent is invalid";
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
        LOG_WARN << "Channel::delListenEvents() IOEvent is invalid";
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
        LOG_INFO << "set nonblock success, fd = " << m_fd;
    } else {
        LOG_ERROR << "set nonblock failed, fd = " << m_fd
            << " strerror = " << strerror(errno);
    }
}

void Channel::removeFromLoop() {
    m_loop->removeChannel(this);
}

void Channel::updateToLoop() {
    m_loop->updateChannel(this);
}

void Channel::handleEventWithGuard() {
    if(m_revents & IOEvent::HUP) {
        if(m_closeCallback) {
            m_closeCallback();
        }
    }
    if(m_revents & IOEvent::ERROR) {
        if(m_errorCallback) {
            m_errorCallback();
        }
    }
    if(m_revents & IOEvent::READ) {
        if(m_readCallback) {
            m_readCallback();
        }
    }
    if(m_revents & IOEvent::WRITE) {
        if(m_writeCallback) {
            m_writeCallback();
        }
    }
}

// ChannelManager
ChannelManager::ChannelManager(int32_t size /*= 256*/)
    : m_size(size),
      m_channels(size + 1) {
}

Channel::ptr ChannelManager::getChannel(int32_t fd, EventLoop *loop /*= nullptr*/) {
    MutexType::Lock lock(m_mutex);
    int32_t size = (int32_t)m_channels.size();
    if(fd > (int32_t)m_channels.size()) {
        int32_t newSize = (size * 1.5) <= fd ? fd : size * 1.5;
        m_channels.resize(newSize + 1);
    }

    loop = loop != nullptr ? loop : EventLoop::GetEventLoop();
    if(m_channels[fd]) {
        if(m_channels[fd]->getEventLoop() != loop) {
            LOG_WARN << "getChannel exist, fd = " << fd << ", loop = " 
                << m_channels[fd]->getEventLoop() << ", now loop = " << loop
                << ", don't worry! it may be trying to wake up the loop[" 
                << m_channels[fd]->getEventLoop() << "]";
        }
        return m_channels[fd];
    }

    m_channels[fd] = std::make_shared<Channel>(loop, fd);
    LOG_INFO << "Channel Manager new a channel, fd = " << fd; 
    return m_channels[fd];
}

}   // namespace shero