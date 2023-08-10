#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/base/Thread.h"

#include <assert.h>

namespace shero {

Thread::Thread(Callback cb, const std::string &name /*= ""*/)
    : m_start(false),
      m_name(name),
      m_cb(std::move(cb)) {
    sem_init(&m_sem, 0, 0);
}

Thread::~Thread() {
    LOG_INFO << "~Thread " << m_tid;
    join();
}

void Thread::start() {
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    assert(rt == 0);

    rt = sem_wait(&m_sem);
    assert(rt == 0);
}

void Thread::join() {
    if(m_start) {
        int rt = pthread_join(m_thread, nullptr);
        assert(rt == 0);
        m_start = false;
    }
}

void *Thread::run(void *arg) {
    Thread *thread = (Thread *)arg;
    thread->m_start = true;
    thread->m_tid = GetThreadId();

    int rt = sem_post(&thread->m_sem);
    assert(rt == 0);

    thread->m_cb();

    return nullptr;
}

}   // namespce shero