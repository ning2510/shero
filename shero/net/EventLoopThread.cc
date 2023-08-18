#include "shero/net/EventLoop.h"
#include "shero/net/EventLoopThread.h"

#include <assert.h>

using namespace shero;

EventLoopThread::EventLoopThread(
        const ThreadInitCallback &cb /*= nullptr*/,
        const std::string &name /*= ""*/)
        : m_name(name),
          m_thread(std::bind(&EventLoopThread::threadFunc, this), name),
          m_loop(nullptr),
          m_cb(cb) {
    int32_t rt = pthread_cond_init(&m_cond, nullptr);
    assert(rt == 0);
}

EventLoopThread::~EventLoopThread() {
    // LOG_DEBUG << "~EventLoopThread";
    if(m_loop != nullptr) {
        m_loop->quit();
        m_thread.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!m_thread.isStart());
    m_thread.start();

    EventLoop* loop = nullptr;
    {
        MutexLockGuard lock(m_mutex);
        while (m_loop == nullptr) {
            pthread_cond_wait(&m_cond, m_mutex.getPthreadMutex());
        }
        loop = m_loop;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if(m_cb) {
        m_cb(&loop);
    }

    {
        MutexLockGuard lock(m_mutex);
        m_loop = &loop;
        pthread_cond_signal(&m_cond);
    }

  loop.loop();
  MutexLockGuard lock(m_mutex);
  m_loop = nullptr;
}

