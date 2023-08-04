#ifndef __SHERO_MUTEX_H
#define __SHERO_MUTEX_H

#include "shero/base/Noncopyable.h"

#include <pthread.h>

namespace shero {

template <class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T &mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    void lock() {
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    ~ScopedLockImpl() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    bool m_locked;
    T &m_mutex;
};

class Mutex : public Noncopyable {
public:
    typedef ScopedLockImpl<Mutex> Lock;

    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_t *getMutex() {
        return &m_mutex;
    }

    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

template <class T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T &mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    void lock() {
        if(!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    ~ReadScopedLockImpl() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    bool m_locked;
    T &m_mutex;
};

template <class T>
class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T &mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    void lock() {
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    ~WriteScopedLockImpl() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    bool m_locked;
    T &m_mutex;
};

class RWMutex : public Noncopyable {
public:
    typedef ReadScopedLockImpl<RWMutex> WriteLock;
    typedef WriteScopedLockImpl<RWMutex> ReadLock;

    RWMutex() {
        pthread_rwlock_init(&m_mutex, nullptr);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_mutex);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_mutex);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_mutex);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_mutex);
    }

private:
    pthread_rwlock_t m_mutex;
};

}   // namespace shero

#endif
