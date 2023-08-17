#ifndef __SHERO_POLLER_H
#define __SHERO_POLLER_H

#include "shero/base/Noncopyable.h"

#include <vector>
#include <unordered_map>

namespace shero {

class Channel;
class EventLoop;

class Poller : public Noncopyable {
public:
    typedef std::vector<Channel *> ChannelList;
    // <fd, Channel *>
    typedef std::unordered_map<int, Channel *> ChannelMap;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    virtual void poll(int32_t timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;
    
    virtual bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);
    void assertInLoopThread() const;

protected:
    ChannelMap m_channelMap;

private:
    EventLoop *m_loop;
};

}   // namespace shero

#endif
