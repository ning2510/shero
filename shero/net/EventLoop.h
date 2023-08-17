#ifndef __SHERO_EVENTLOOP_H
#define __SHERO_EVENTLOOP_H

// #include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/base/Mutex.h"
#include "shero/net/Channel.h"
#include "shero/net/EPollPoller.h"

#include <vector>
#include <memory>
#include <atomic>

namespace shero {

class EventLoop : public Noncopyable {
public:
    typedef std::shared_ptr<EventLoop> ptr;
    typedef RWMutex RWMutexType;
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();  

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    void loop();
    void quit();
    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);
    bool isInLoopThread() { return m_tid == GetThreadId(); }
    void assertInLoopThread() {
        if(!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    void wakeup();

    pid_t getThreadId() const { return m_tid; }
    int32_t getWakeupFd() const { return m_wakeupFd; }

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

private:
    typedef std::vector<Channel *> ChannelList;
    static const int m_pollTimeMs;

    pid_t m_tid;
    bool m_looping;
    RWMutexType m_mutex;

    int32_t m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChannel;
    std::unique_ptr<Poller> m_poller;
    
    bool m_eventHandling;
    bool m_callingpendingFunctors;

    Channel* m_currentActiveChannel;
    ChannelList m_activeChannels;
    std::vector<Functor> m_pendingFunctors;
};

}  // namespace shero

#endif
