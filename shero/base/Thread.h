#ifndef __SHERO_THREAD_H
#define __SHERO_THREAD_H

#include "shero/base/Noncopyable.h"

#include <memory>
#include <pthread.h>
#include <functional>
#include <semaphore.h>
#include <condition_variable>

namespace shero {

class Thread : public Noncopyable {
public:
    typedef std::shared_ptr<Thread> ptr;
    typedef std::function<void()> Callback;

    Thread(Callback cb, const std::string &name = "");
    ~Thread();
    void start();
    void join();

    pid_t getTid() const { return m_tid; }
    bool isStart() const { return m_start; }

private:
    static void *run(void *arg);

private:
    pid_t m_tid;
    pthread_t m_thread;
    sem_t m_sem;
    bool m_start;
    std::string m_name;

    Callback m_cb;
};

}   // namespace shero

#endif
