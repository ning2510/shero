#include "shero/base/Log.h"
#include "shero/base/Mutex.h"
#include "shero/net/Poller.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <sys/eventfd.h>

namespace shero {

static thread_local EventLoop *t_eventLoop = nullptr;

const int EventLoop::m_pollTimeMs = 100000;

int32_t createEventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        LOG_FATAL << "eventfd error : " << errno;
    } else {
        LOG_INFO << "eventFd() success, event fd = " << evtfd;
    }
    return evtfd;
}

EventLoop::EventLoop()
    : m_tid(GetThreadId()),
      m_looping(false),
      m_wakeupFd(createEventFd()),
      m_wakeupChannel(new Channel(this, m_wakeupFd)),
      m_poller(Poller::newDefaultPoller(this)),
      m_eventHandling(false),
      m_callingpendingFunctors(false),
      m_currentActiveChannel(nullptr) {

    if (t_eventLoop) {
        LOG_FATAL << "Another EventLoop " << t_eventLoop
                  << " exists in this thread " << m_tid;
    } else {
        t_eventLoop = this;
    }

    m_wakeupChannel->setReadCallback(
        std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->addListenEvents(IOEvent::READ);
}

EventLoop::~EventLoop() {
    m_wakeupChannel->delAllListenEvents();
    m_wakeupChannel->removeFromLoop();
    close(m_wakeupFd);
    t_eventLoop = nullptr;
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->getEventLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->getEventLoop() == this);
    assertInLoopThread();
    if (m_eventHandling) {
        assert(m_currentActiveChannel == channel ||
            std::find(m_activeChannels.begin(), 
            m_activeChannels.end(), channel) == m_activeChannels.end());
    }

    m_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
    assert(channel->getEventLoop() == this);
    assertInLoopThread();
    return m_poller->hasChannel(channel);
}

void EventLoop::loop() {
    assert(!m_looping);
    m_looping = true;

    LOG_DEBUG << "EventLoop [" << this << "] start loop";

    while (m_looping) {
        m_activeChannels.clear();
        m_poller->poll(m_pollTimeMs, &m_activeChannels);

        m_eventHandling = true;
        for(Channel *channel : m_activeChannels) {
            m_currentActiveChannel = channel;
            m_currentActiveChannel->handleEvent();
        }
        m_currentActiveChannel = nullptr;
        m_eventHandling = false;
    
        // FIXME: SIGSEGV occasionally
        try {
            doPendingFunctors();
        } catch(...) {
            LOG_ERROR << "something error in EventLoop::doPendingFunctors()";
        }
    }
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop[" << this 
        << "], its thread id = " << m_tid << ", current thread id = " << GetThreadId();
}

void EventLoop::quit() {
    m_looping = false;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if(isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        RWMutex::WriteLock lock(m_mutex);

        // FIXME: SIGSEGV occasionally
        try {
            m_pendingFunctors.push_back(std::move(cb));
        } catch(...) {
            LOG_ERROR << "something error in EventLoop::queueInLoop()";
        }
    }

    if(!isInLoopThread() || m_callingpendingFunctors) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    
    {
        RWMutexType::ReadLock lock(m_mutex);
        if(m_pendingFunctors.empty()) {
            return ;
        }
    }

    m_callingpendingFunctors = true;
    {
        RWMutexType::WriteLock lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for(const Functor &functor : functors) {
        if(functor) {
            functor();
        }
    }

    m_callingpendingFunctors = false;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    if(write(m_wakeupFd, &one, sizeof(one)) != sizeof(one)) {
        LOG_ERROR << "EventLoop::wakeup() error, strerror = " << strerror(errno);
    }
    LOG_INFO << "EventLoop::wakeup() write, event loop = " << this;
}

void EventLoop::handleRead() {
    uint64_t one;
    if(read(m_wakeupFd, &one, sizeof(one)) != sizeof(one)) {
        LOG_ERROR << "EventLoop::handleRead() error, strerror = " << strerror(errno);
    }
    LOG_INFO << "EventLoop::wakeup() read, event loop = " << this;
}


} // namespace shero