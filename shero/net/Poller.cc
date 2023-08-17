#include "shero/net/Poller.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"
#include "shero/net/EPollPoller.h"

namespace shero {

Poller::Poller(EventLoop *loop)
    : m_loop(loop) {
}

bool Poller::hasChannel(Channel *channel) const {
    assertInLoopThread();
    auto it = m_channelMap.find(channel->getFd());
    return it != m_channelMap.end() && it->second == channel;
}

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    return new EPollPoller(loop);
}

void Poller::assertInLoopThread() const {
    m_loop->assertInLoopThread();
}

}   // namespace shero