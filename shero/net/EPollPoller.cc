#include "shero/base/Log.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"
#include "shero/net/EPollPoller.h"

#include <string.h>
#include <unistd.h>

namespace shero {

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      m_epfd(epoll_create1(EPOLL_CLOEXEC)),
      m_events(m_epollEventSize) {
    if(m_epfd < 0) {
        LOG_ERROR << "epoll_create1() error, strerror = " << strerror(errno);
    }
}

EPollPoller::~EPollPoller() {
    close(m_epfd);
}

void EPollPoller::poll(int32_t timeoutMs, ChannelList *activeChannels) {
    int32_t numEvents = epoll_wait(m_epfd, &(*m_events.begin()), 
                static_cast<int32_t>(m_events.size()), -1);
    
    if(numEvents > 0) {
        LOG_DEBUG << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == static_cast<int32_t>(sizeof(m_events))) {
            m_events.resize(m_events.size() * 2);
        }
    } else if(numEvents == 0) {
        LOG_WARN << "epoll_wait() timeout";
    } else {
        LOG_ERROR << "epoll_wait() error, strerror = " << strerror(errno);
    }
}

void EPollPoller::updateChannel(Channel *channel) {
    ChannelStatus status = channel->getStatus(); 
    if(status == ChannelStatus::NEW || status == ChannelStatus::DELETE) {
        m_channelMap[channel->getFd()] = channel;
        channel->setStatus(ChannelStatus::ADD);
        update(EPOLL_CTL_ADD, channel);
    } else {
        if(channel->isNoneEvent()) {
            removeChannel(channel);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel) {
    ChannelStatus status = channel->getStatus();
    if(status != ChannelStatus::ADD) {
        return ;
    }

    m_channelMap.erase(channel->getFd());
    channel->setStatus(ChannelStatus::DELETE);
    update(EPOLL_CTL_DEL, channel);
}

void EPollPoller::update(int32_t opt, Channel *channel) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.events = channel->getEvent();
    ev.data.ptr = channel;

    int rt = epoll_ctl(m_epfd, opt, channel->getFd(), &ev);
    if(rt < 0) {
        LOG_ERROR << "epoll_ctl error, strerror = " << strerror(errno);
    }
}

void EPollPoller::fillActiveChannels(int32_t numEvents, ChannelList *activeChannels) {
    for(int i = 0; i < numEvents; i++) {
        epoll_event ev = m_events[i];
        Channel *channel = static_cast<Channel *>(ev.data.ptr);
        channel->setRevents(ev.events);
        activeChannels->push_back(channel);
    }
}

}   // namespace shero