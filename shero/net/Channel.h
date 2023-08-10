#ifndef __SHERO_CHANNEL_H
#define __SHERO_CHANNEL_H

#include "shero/base/Mutex.h"
#include "shero/base/Singleton.h"
#include "shero/base/Noncopyable.h"
#include "shero/coroutine/Coroutine.h"

#include <vector>
#include <memory>
#include <functional>
#include <sys/epoll.h>

namespace shero {

class EventLoop;

enum IOEvent {
    NONE = 0,
    READ = EPOLLIN,
    WRITE = EPOLLOUT,
    ERROR = EPOLLERR,
    HUP = EPOLLHUP
};

enum ChannelStatus {
    NEW = 0,    // Channel 未加入 Poller
    ADD = 1,    // Channel 已加入 Poller
    DELETE = 2  // Channel 已从 Poller 中删除
};

class Channel : public Noncopyable {
public:
    typedef std::shared_ptr<Channel> ptr;
    Channel(EventLoop *loop, int32_t fd);
    ~Channel();

    void addListenEvents(IOEvent event);
    void delListenEvents(IOEvent event);
    void delAllListenEvents();

    void handleEvent();
    void tie(const std::shared_ptr<void> &v);
    void setNonBlock();
    void removeFromLoop();

    int32_t getFd() const { return m_fd; }
    int32_t getEvent() const { return m_event; }
    EventLoop *getEventLoop() const { return m_loop; }

    int32_t getRevents() const { return m_revents; }
    void setRevents(int32_t revents) { m_revents = revents; }

    ChannelStatus getStatus() const { return m_status; }
    void setStatus(ChannelStatus status) { m_status = status; }

    void setReadCallback(std::function<void()> cb) { m_readCallback = std::move(cb); }
    void setWriteCallback(std::function<void()> cb) { m_writeCallback = std::move(cb); }
    void setErrorCallback(std::function<void()> cb) { m_errorCallback = std::move(cb); }
    void setCloseCallback(std::function<void()> cb) { m_closeCallback = std::move(cb); }

    bool isReading() { return m_event & IOEvent::READ; }
    bool isWriting() { return m_event & IOEvent::WRITE; }
    bool isNoneEvent() { return m_event == IOEvent::NONE; }

    void setCoroutine(Coroutine *cor) { m_cor = cor; }
    Coroutine *getCoroutine() const { return m_cor; }
    void clearCoroutine() { m_cor = nullptr; }

private:
    void updateToLoop();
    void handleEventWithGuard();

private:
    int32_t m_fd;
    int32_t m_event;
    int32_t m_revents;
    EventLoop *m_loop;
    Coroutine *m_cor;

    ChannelStatus m_status;

    bool m_tied;
    std::weak_ptr<void> m_tie;

    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    std::function<void()> m_errorCallback;
    std::function<void()> m_closeCallback;
};

class ChannelManager {
public:
    typedef std::shared_ptr<ChannelManager> ptr;
    typedef Mutex MutexType;
    ChannelManager(int32_t size = 256);

    Channel::ptr getChannel(int32_t fd);

private:
    MutexType m_mutex;
    int32_t m_size;
    std::vector<Channel::ptr> m_channels;
};

typedef Singleton<ChannelManager> ChannelMgr;

}   // namespace shero

#endif
