#ifndef __SHERO_FD_EVENT_H
#define __SHERO_FD_EVENT_H

#include "shero/net/EventLoop.cc"

#include <memory>
#include <functional>
#include <sys/epoll.h>

namespace shero {

enum class IOEvent {
    READ = EPOLLIN,
    WRITE = EPOLLOUT
};

class FdEvent {
public:
    typedef std::shared_ptr<FdEvent> ptr;
    FdEvent();
    virtual ~FdEvent();

    std::function<void()> getCallback(IOEvent v) {
        if(v == IOEvent::READ) {
            return m_readCb;
        } else if(v == IOEvent::WRITE) {
            return m_writeCb;
        }
    }

protected:
    int32_t m_fd;

    std::function<void()> m_readCb;
    std::function<void()> m_writeCb;

};

}   // naemspace shero

#endif
