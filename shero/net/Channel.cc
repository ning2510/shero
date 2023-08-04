#include "shero/base/Log.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"

#include <unistd.h>

namespace shero {

Channel::Channel(EventLoop *loop, int32_t fd)
    : m_fd(fd),
      m_event(IOEvent::NONE),
      m_revents(IOEvent::NONE),
      m_loop(loop),
      m_status(ChannelStatus::NEW),
      m_tied(false) {
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

void Channel::tie(std::shared_ptr<void> &v) {
    if(v) {
        m_tie = v;
        m_tied = true;
    }
}

void Channel::removeFromLoop() {
    m_loop->removeChannel(this);
}

void Channel::updateToLoop() {
    m_loop->updateChannel(this);
}

void Channel::handleEventWithGuard() {
    if(m_revents & EPOLLHUP) {
        if(m_closeCallback) {
            m_closeCallback();
        }
    }
    if(m_revents & EPOLLERR) {
        if(m_errorCallback) {
            m_errorCallback();
        }
    }
    if(m_revents & EPOLLIN) {
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