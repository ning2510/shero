#include "shero/Log.h"
#include "shero/Util.h"
#include "shero/Timer.h"

#include <vector>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/timerfd.h>

namespace shero {

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

TimerEvent::TimerEvent(int64_t interval, std::function<void()> cb, bool recycle, Timer *timer)
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
Timer::Timer() {
    m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(m_fd < 0) {
        LOG_ERROR << "timerfd_create error, strerror = " << strerror(errno);
    }

    // TODO: register in epoll
    
    m_readCb = std::bind(&Timer::onTimer, this);
}

Timer::~Timer() {
    // TODO: unregister from epoll

    close(m_fd);
}

void Timer::addTimer(TimerEvent::ptr timer) {
    bool reset = false;

    RWMutexType::WriteLock lock(m_mutex);
    if(m_timers.empty() || timer->getArrive() < (*m_timers.begin())->getArrive()) {
        reset = true;
    }
    m_timers.insert(timer);
    lock.unlock();

    if(reset) {
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
    RWMutexType::ReadLock rlock(m_mutex);
    auto it = m_timers.find(timer);
    if(it == m_timers.end()) {
        return ;
    }
    rlock.unlock();

    RWMutexType::WriteLock wlock(m_mutex);
    // if the first timer is deleted
    bool reset = (it == m_timers.begin());
    m_timers.erase(it);
    // and the arrival time of this timer is greater than the arrival 
    // time of the next timer, then call resetArriveTime()
    reset = reset && (*it)->getArrive() < (*m_timers.begin())->getArrive();
    wlock.unlock();

    if(reset) {
        resetArriveTime();
    }
}

bool Timer::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

void Timer::resetArriveTime() {
    RWMutexType::ReadLock rlock(m_mutex);
    if(m_timers.empty()) {
        return ;
    }
    rlock.unlock();

    uint64_t now = GetCurrentMS();
    RWMutexType::WriteLock wlock(m_mutex);
    while((*m_timers.begin())->getArrive() < now) {
        auto it = m_timers.begin();
        m_timers.erase(it);
    }

    uint64_t diff = (*m_timers.begin())->getArrive() - now;
    wlock.unlock();

    struct itimerspec ts;
    bzero(&ts, sizeof(ts));
    ts.it_value.tv_sec = diff / 1000;
    ts.it_value.tv_nsec = (diff % 1000) * 1000 * 1000;

    // flags is 0: relative time
    int rt = timerfd_settime(m_fd, 0, &ts, nullptr);
    if(rt < 0) {
        LOG_ERROR << "timerfd_settime error, strerror = " << strerror(errno);
    }
}

void Timer::onTimer() {
    uint8_t x;
    while(read(m_fd, &x, sizeof(x)) == 1);

    uint64_t now = GetCurrentMS();
    std::vector<TimerEvent::ptr> recyTimer;
    std::vector<TimerEvent::ptr> readyTimer;
    
    RWMutexType::WriteLock lock(m_mutex);
    auto it = m_timers.begin();
    for(it = m_timers.begin(); it != m_timers.end(); it++) {
        if((*it)->getArrive() > now) break;
        if((*it)->isRecycle()) {
            (*it)->resetArrive();
            recyTimer.push_back(*it);
        }
        readyTimer.push_back(*it);
    }
    it = (*it)->getArrive() <= now ? m_timers.end() : it;
    m_timers.erase(m_timers.begin(), it);
    lock.unlock();

    for(auto &i : recyTimer) {
        addTimer(i);
    }

    resetArriveTime();

    for(auto &i : readyTimer) {
        if(i->m_cb) {
            i->m_cb();
        }
    }
}

}   // namespace shero