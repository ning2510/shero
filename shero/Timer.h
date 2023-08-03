#ifndef __SHERO_TIMER_H
#define __SHERO_TIMER_H

#include "shero/Mutex.h"
#include "shero/net/FdEvent.h"

#include <set>
#include <memory>
#include <functional>

namespace shero {

class Timer;

class TimerEvent : public std::enable_shared_from_this<TimerEvent> {
friend class Timer;
public:
    typedef std::shared_ptr<TimerEvent> ptr;

    // 删除定时器
    void cancel();
    // 更新定时器周期，startNow = true 以当前时间为开始时间
    void resetInterval(uint64_t interval, bool startNow = true);
    // 更新定时器触发时间，以当前时间为开始时间
    void resetArrive();
    uint64_t getArrive() const { return m_arrive; }

    bool isRecycle() const { return m_recycle; }
    void setRecycle(bool v) { m_recycle = v; }


private:
    TimerEvent(int64_t interval, std::function<void()> cb, 
        bool recycle, Timer *timer);

    struct Comparator {
        bool operator()(const TimerEvent::ptr &l, const TimerEvent::ptr &r);
    };

private:
    bool m_recycle;
    uint64_t m_arrive;
    uint64_t m_interval;
    std::function<void()> m_cb;
    Timer *m_timer;
};

class Timer : public FdEvent {
friend class TimerEvent;
public:
    typedef std::shared_ptr<Timer> ptr;
    typedef RWMutex RWMutexType;
    Timer();
    ~Timer();

    void addTimer(TimerEvent::ptr timer);
    TimerEvent::ptr addTimer(uint64_t interval, 
        std::function<void()> cb, bool recycle = false);

    void delTimer(TimerEvent::ptr timer);
    bool hasTimer();

    void resetArriveTime();
    // Timer 回调函数，内部执行 TimerEvent 的 m_cb
    void onTimer();

private:
    RWMutexType m_mutex;

    // 从小到大
    std::set<TimerEvent::ptr, TimerEvent::Comparator> m_timers;
};

}   // namespace shero

#endif
