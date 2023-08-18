#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/base/Timer.h"
#include "shero/net/EventLoop.h"

#include <vector>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/timerfd.h>
#include <iostream>

namespace shero {

int32_t createTimerFd() {
    int32_t fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(fd < 0) {
        LOG_FATAL << "timerfd_create error, strerror = " << strerror(errno);
    }
    LOG_DEBUG << "timerfd_create susscess, fd = " << fd;
    return fd;
}

// TimerEvent
bool TimerEvent::Comparator::operator()(const TimerEvent::ptr &l, const TimerEvent::ptr &r) {
    if(!l && !r) {
        return false;
    }
    if(!l || !r) {
        return !l ? true : false;
    }
    if(l->getArrive() < r->getArrive()) {
        return true;
    }
    return l->getArrive() > r->getArrive() ? false : l.get() < r.get();
}

TimerEvent::TimerEvent(uint64_t interval, std::function<void()> cb, bool recycle, Timer *timer)
    : m_recycle(recycle),
      m_arrive(GetCurrentMS() + interval),
      m_interval(interval),
      m_cb(std::move(cb)),
      m_timer(timer) {
}

void TimerEvent::cancel() {
    m_timer->delTimer(shared_from_this());
}

void TimerEvent::resetInterval(uint64_t interval, bool startNow /*= true*/) {
    if(interval == m_interval && !startNow) {
        return ;
    }

    cancel();

    uint64_t startTime = startNow ? GetCurrentMS() : m_arrive - m_interval;
    m_arrive = startTime + interval;
    m_interval = interval;

    m_timer->addTimer(shared_from_this());
}

void TimerEvent::resetArrive() {
    m_arrive = GetCurrentMS() + m_interval;
}

// Timer
Timer::Timer(EventLoop *loop)
    : m_fd(createTimerFd()),
      m_loop(loop) {

    m_channel = ChannelMgr::GetInstance()->getChannel(m_fd, loop);
    m_channel->setReadCallback(std::bind(&Timer::onTimer, this));
    m_loop->runInLoop(std::bind(&Timer::timerCreated, this));
}

Timer::~Timer() {
    LOG_INFO << "~Timer";
    close(m_fd);
}

void Timer::timerCreated() {
    m_channel->getEventLoop()->assertInLoopThread();
    m_channel->addListenEvents(IOEvent::READ);
}

// external call
void Timer::timerDestroyed() {
    m_channel->getEventLoop()->assertInLoopThread();
    m_channel->delAllListenEvents();
    m_channel->removeFromLoop();
}

void Timer::addTimer(TimerEvent::ptr timer, bool reset /*= true*/) {
    bool need = false;

    {
        MutexLockGuard lock(m_mutex);
        if(m_timers.empty() || timer->getArrive() < (*m_timers.begin())->getArrive()) {
            need = true;
        }
        m_timers.insert(timer);
    }

    if(need && reset) {
        resetArriveTime();
    }
}

TimerEvent::ptr Timer::addTimer(uint64_t interval, 
        std::function<void()> cb, bool recycle /*= false*/) {
    TimerEvent::ptr timer = std::make_shared<TimerEvent>(interval, cb, recycle, this);
    addTimer(timer);
    return timer;
}

void Timer::delTimer(TimerEvent::ptr timer) {
    bool reset;
    {
        MutexLockGuard lock(m_mutex);
        auto it = m_timers.find(timer);
        if(it == m_timers.end()) {
            return ;
        }

        reset = (it == m_timers.begin());
        // if the first timer is deleted
        m_timers.erase(it);
        // and the arrival time of this timer is greater than the arrival 
        // time of the next timer, then call resetArriveTime()
        reset = reset && (*it)->getArrive() < (*m_timers.begin())->getArrive();
    }

    if(reset) {
        resetArriveTime();
    }
}

bool Timer::hasTimer() {
    MutexLockGuard lock(m_mutex);
    return !m_timers.empty();
}

void Timer::resetArriveTime() {
    {
        MutexLockGuard lock(m_mutex);
        if(m_timers.empty()) {
            return ;
        }
    }

    uint64_t now = GetCurrentMS();
    uint64_t diff;
    {
        MutexLockGuard lock(m_mutex);
        while(!m_timers.empty() && (*m_timers.begin())->getArrive() < now) {
            auto it = m_timers.begin();
            m_timers.erase(it);
        }

        if(m_timers.empty()) {
            return ;
        }

        diff = (*m_timers.begin())->getArrive() - now;
    }

    struct itimerspec ts;
    bzero(&ts, sizeof(ts));
    ts.it_value.tv_sec = diff / 1000;
    ts.it_value.tv_nsec = (diff % 1000) * 1000 * 1000;

    // flags is 0: relative time
    int rt = timerfd_settime(m_fd, 0, &ts, nullptr);
    if(rt < 0) {
        LOG_ERROR << "timerfd_settime error, strerror = " << strerror(errno);
    }
    LOG_DEBUG << "timerfd_settime success, fd = " << m_fd;
}

void Timer::onTimer() {
    uint8_t x;
    while(read(m_fd, &x, sizeof(x)) == 1);

    uint64_t now = GetCurrentMS();
    std::vector<TimerEvent::ptr> recyTimer;
    std::vector<TimerEvent::ptr> readyTimer;

    {
        MutexLockGuard lock(m_mutex);
        auto it = m_timers.begin();
        for(it = m_timers.begin(); it != m_timers.end(); it++) {
            if((*it)->getArrive() > now) break;
            if((*it)->isRecycle()) {
                (*it)->resetArrive();
                recyTimer.push_back(*it);
            }
            readyTimer.push_back(*it);
        }
        m_timers.erase(m_timers.begin(), it);
    }

    for(auto &i : recyTimer) {
        // 这里 addTimer 时 reset 如果为 true，则有可能执行两次 resetArriveTime()
        addTimer(i, false);
    }

    // Timer 回调触发后一定会 resetArriveTime()
    resetArriveTime();

    for(auto &i : readyTimer) {
        if(i->m_cb) {
            i->m_cb();
        }
    }
}

}   // namespace shero