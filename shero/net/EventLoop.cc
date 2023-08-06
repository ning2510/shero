#include "shero/base/Log.h"
#include "shero/base/Macro.h"
#include "shero/net/Poller.h"
#include "shero/net/EventLoop.h"

#include <string.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <iostream>

namespace shero {

static thread_local EventLoop::ptr t_eventLoop_ptr = nullptr;
static thread_local EventLoop *t_eventLoop = nullptr;

const int EventLoop::m_pollTimeMs = 100000;

EventLoop *EventLoop::GetEventLoop() {
    if(SHERO_UNLICKLY(!t_eventLoop_ptr)) {
        t_eventLoop_ptr.reset(new EventLoop());
        t_eventLoop = t_eventLoop_ptr.get();
    }
    return t_eventLoop;
}

EventLoop::EventLoop()
    : m_tid(GetThreadId()),
      m_looping(false),
      m_wakeupFd(eventfd(0, EFD_CLOEXEC)),
      m_wakeupChannel(new Channel(this, m_wakeupFd)),
      m_poller(Poller::newDefaultPoller(this)),
      m_callingpendingFunctors(false) {

    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::wakeup, this));
    m_wakeupChannel->addListenEvents(IOEvent::READ);
}

EventLoop::~EventLoop() {
    std::cout << "~EventLoop\n";
    t_eventLoop_ptr.reset();
    t_eventLoop = nullptr;
}

void EventLoop::updateChannel(Channel *channel) {
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    m_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return m_poller->hasChannel(channel);
}

void EventLoop::loop() {
    m_looping = true;
    
    LOG_DEBUG << "EventLoop " << this << " start loop";

    while(m_looping) {
        m_activeChannels.clear();
        m_poller->poll(m_pollTimeMs, &m_activeChannels);

        for(auto &channel : m_activeChannels) {
            channel->handleEvent();
        }

        doPendingFunctors();
    }

    LOG_INFO << "EventLoop [" << this << "] loop end";
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
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_pendingFunctors.emplace_back(cb);
    }

    if(!isInLoopThread() || m_callingpendingFunctors) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    
    m_callingpendingFunctors = true;
    {
        RWMutexType::WriteLock lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for(auto &functor : functors) {
        functor();
    }

    m_callingpendingFunctors = false;
}

void EventLoop::wakeup() {
    uint8_t one = 1;
    if(write(m_wakeupFd, &one, sizeof(one)) != sizeof(one)) {
        LOG_ERROR << "EventLoop::wakeup() error, strerror = " << strerror(errno);
    }
}

void EventLoop::handleRead() {
    uint8_t one;
    if(read(m_wakeupFd, &one, sizeof(one)) != sizeof(one)) {
        LOG_ERROR << "EventLoop::handleRead() error, strerror = " << strerror(errno);
    }
}

}   // namespace shero