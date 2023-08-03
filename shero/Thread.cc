#include "shero/Util.h"
#include "shero/Thread.h"

#include <assert.h>
#include <iostream>

namespace shero {

Thread::Thread(Callback cb) 
    : m_start(false),
      m_cb(std::move(cb)) {
    sem_init(&m_sem, 0, 0);
    start();
}

Thread::~Thread() {
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
    int rt = sem_post(&thread->m_sem);
    assert(rt == 0);

    thread->m_start = true;
    thread->m_tid = GetThreadId();
    thread->m_cb();

    return nullptr;
}

}   // namespce shero