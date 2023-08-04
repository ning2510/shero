#ifndef __SHERO_EVENTLOOP_H
#define __SHERO_EVENTLOOP_H

#include "shero/base/Util.h"
#include "shero/base/Mutex.h"
#include "shero/net/Channel.h"
#include "shero/net/EPollPoller.h"

#include <vector>
#include <memory>
#include <atomic>

namespace shero {

class EventLoop {
public:
    typedef RWMutex RWMutexType;
    typedef std::function<void()> Functor;

    static EventLoop *GetEventLoop();

    EventLoop();
    ~EventLoop();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    void loop();
    void quit();
    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);
    bool isInLoopThread() { return m_tid == GetThreadId(); }

    void wakeup();

private:
    void handleRead();
    void doPendingFunctors();

private:
    typedef std::vector<Channel *> ChannelList;
    static const int m_pollTimeMs;

    pid_t m_tid;
    std::atomic_bool m_looping;
    RWMutexType m_mutex;

    int32_t m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChannel;
    std::unique_ptr<Poller> m_poller;

    ChannelList m_activeChannels;
    std::atomic_bool m_callingpendingFunctors;
    std::vector<Functor> m_pendingFunctors;
};

}   // namespace shero

#endif
