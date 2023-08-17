#ifndef __SHERO_EVENTLOOPTHREAD_H
#define __SHERO_EVENTLOOPTHREAD_H

#include "shero/base/Mutex.h"
#include "shero/base/Thread.h"
#include "shero/base/Noncopyable.h"

#include <memory>
#include <pthread.h>
#include <functional>

namespace shero {

class EventLoop;

class EventLoopThread : public Noncopyable {
public:
    typedef std::shared_ptr<EventLoopThread> ptr;
    typedef Mutex MutexType;
    typedef std::function<void(EventLoop *)> ThreadInitCallback;
    EventLoopThread(const ThreadInitCallback &cb = nullptr,
        const std::string &name = "");
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();

private:
    std::string m_name;
    Thread m_thread;
    EventLoop *m_loop;
    ThreadInitCallback m_cb;
    MutexType m_mutex;
    pthread_cond_t m_cond;
};

}   // namespace shero

#endif
