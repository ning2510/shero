// #include "shero/base/Log.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"
#include "shero/net/EPollPoller.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

namespace shero {

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      m_epfd(epoll_create1(EPOLL_CLOEXEC)),
      m_events(m_epollEventSize) {
    if(m_epfd < 0) {
        // LOG_ERROR << "epoll_create1() error, strerror = " << strerror(errno);
    }
}

EPollPoller::~EPollPoller() {
    close(m_epfd);
}

void EPollPoller::poll(int32_t timeoutMs, ChannelList *activeChannels) {
    int32_t numEvents = epoll_wait(m_epfd, &(*m_events.begin()), 
                static_cast<int32_t>(m_events.size()), timeoutMs);
    
    if(numEvents > 0) {
        // LOG_DEBUG << numEvents << " events happened";
        
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == static_cast<int32_t>(sizeof(m_events))) {
            m_events.resize(m_events.size() * 2);
        }
    } else if(numEvents == 0) {
        // LOG_WARN << "epoll_wait() timeout";
    } else {
        // LOG_ERROR << "epoll_wait() error, strerror = " << strerror(errno);
    }
}

void EPollPoller::updateChannel(Channel* channel) {
    Poller::assertInLoopThread();
    ChannelStatus status = channel->getStatus();
    if (status == ChannelStatus::NEW || status == ChannelStatus::DELETE) {
        int32_t fd = channel->getFd();
        if (status == ChannelStatus::NEW) {
            assert(m_channelMap.find(fd) == m_channelMap.end());
            m_channelMap[fd] = channel;
        } else {
            assert(m_channelMap.find(fd) != m_channelMap.end());
            assert(m_channelMap[fd] == channel);
        }

        channel->setStatus(ChannelStatus::ADD);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int32_t fd = channel->getFd();
        assert(m_channelMap.find(fd) != m_channelMap.end());
        assert(m_channelMap[fd] == channel);
        assert(status == ChannelStatus::ADD);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setStatus(ChannelStatus::DELETE);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel) {
    Poller::assertInLoopThread();
    int32_t fd = channel->getFd();
    assert(m_channelMap.find(fd) != m_channelMap.end());
    assert(m_channelMap[fd] == channel);
    assert(channel->isNoneEvent());

    ChannelStatus status = channel->getStatus();
    assert(status == ChannelStatus::ADD || status == ChannelStatus::DELETE);
    
    m_channelMap.erase(fd);
    if(status == ChannelStatus::ADD) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setStatus(ChannelStatus::NEW);
}

void EPollPoller::update(int32_t opt, Channel *channel) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.events = channel->getEvent();
    ev.data.ptr = channel;

    int rt = epoll_ctl(m_epfd, opt, channel->getFd(), &ev);
    if(rt < 0) {
        // LOG_ERROR << "epoll_ctl error, strerror = " << strerror(errno)
        //     << " fd = " << channel->getFd();
    }
}

void EPollPoller::fillActiveChannels(int32_t numEvents, ChannelList *activeChannels) const {
    assert((size_t)numEvents <= m_events.size());
    for(int i = 0; i < numEvents; i++) {
        epoll_event ev = m_events[i];
        Channel *channel = static_cast<Channel *>(ev.data.ptr);

        channel->setRevents(ev.events);
        activeChannels->push_back(channel);

        // LOG_DEBUG << "fd = " << channel->getFd() << " events = " << ev.events
        //     << " channel = " << channel;
    }
}

}   // namespace shero