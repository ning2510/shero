#ifndef __SHERO_EPOLLPOLLER_H
#define __SHERO_EPOLLPOLLER_H

#include "shero/net/Poller.h"

#include <sys/epoll.h>

namespace shero {

class Channel;
class EventLoop;

class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller();

    virtual void poll(int32_t timeoutMs, ChannelList *activeChannels) override;
    virtual void updateChannel(Channel *channel) override;
    virtual void removeChannel(Channel *channel) override;

private:
    void update(int32_t opt, Channel *channel);
    void fillActiveChannels(int32_t numEvents, ChannelList *activeChannels) const;

private:
    typedef std::vector<epoll_event> EventList;
    static const int m_epollEventSize = 16;

    int32_t m_epfd;
    EventList m_events;
};

}   // namespace shero

#endif
