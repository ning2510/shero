#include "shero/net/EventLoopThreadPool.h"
#include "shero/base/Endian.h"

#include <stdio.h>
#include <assert.h>

using namespace shero;

EventLoopThreadPool::EventLoopThreadPool(
        EventLoop *baseLoop, const std::string &nameArg /*= ""*/)
        : m_start(false),
          m_next(0),
          m_numThreads(0),
          m_name(nameArg),
          m_baseLoop(baseLoop) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
    assert(!m_start);
    m_baseLoop->assertInLoopThread();
    m_start = true;

    for (int32_t i = 0; i < m_numThreads; ++i) {
        char buf[m_name.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", m_name.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        m_threads.push_back(std::unique_ptr<EventLoopThread>(t));
        m_loops.push_back(t->startLoop());
        // LOG_DEBUG << "i = " << i << ", m_loops = " << m_loops[m_loops.size() - 1];
    }

    if (m_numThreads == 0 && cb) {
        cb(m_baseLoop);
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
    m_baseLoop->assertInLoopThread();
    assert(m_start);
    EventLoop *loop = m_baseLoop;

    if (!m_loops.empty()) {
        loop = m_loops[m_next];
        ++m_next;
        if ((size_t)m_next >= m_loops.size()) {
            m_next = 0;
        }
    }

    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if(m_loops.empty()) {
        return std::vector<EventLoop *>(1, m_baseLoop);
    }
    return m_loops;
}
