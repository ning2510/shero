#include "shero/base/Log.h"
#include "shero/base/Timer.h"
#include "shero/net/Channel.h"
#include "shero/net/EventLoop.h"
#include "shero/coroutine/Hook.h"
#include "shero/coroutine/Coroutine.h"

#include <dlfcn.h>
#include <iostream>

namespace shero {

static bool g_hook_enable = true;

#define HOOK_FUNC(XX) \
    XX(read) \
    XX(write) \
    XX(socket) \
    XX(accept) \
    XX(connect) \
    XX(sleep)

void hook_init() {
    static bool is_inited = false;
    if(is_inited) {
        return ;
    }
    is_inited = true;

#define XX(name) name##_hook = (name##_hook_func)dlsym(RTLD_NEXT, #name);
    HOOK_FUNC(XX);
#undef XX
}

struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};

static _HookIniter s_hook_initer;

bool is_hook_enable() {
    return g_hook_enable;
}

void set_hook_enable(bool flag) {
    g_hook_enable = flag;
}

}   // namespace shero

extern "C" {
#define XX(name) name##_hook_func name##_hook = nullptr;
    HOOK_FUNC(XX);
#undef XX

ssize_t read(int fd, void *buf, size_t count) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        LOG_DEBUG << "[read] g_hook_enable is " << shero::g_hook_enable 
            << ", current coroutine is main coroutine "
            << shero::Coroutine::IsMainCoroutine();
        return read_hook(fd, buf, count);
    }
    LOG_DEBUG << "read hook start";

    shero::Channel::ptr channel =
         shero::ChannelMgr::GetInstance()->getChannel(fd);
    channel->setNonBlock();

    ssize_t rt = read_hook(fd, buf, count);
    if(rt > 0) {
        return rt;
    }

    shero::Coroutine *cor = shero::Coroutine::GetCurCoroutine();
    channel->setCoroutine(cor);
    channel->addListenEvents(shero::IOEvent::READ);
    channel->setReadCallback([cor]() {
        LOG_DEBUG << "read resume";
        shero::Coroutine::Resume(cor);
    });

    LOG_DEBUG << "read hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Yield";
    shero::Coroutine::Yield();
    LOG_DEBUG << "read hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Resume";

    channel->delListenEvents(shero::IOEvent::READ);
    channel->clearCoroutine();
    
    return read_hook(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        LOG_DEBUG << "[write] g_hook_enable is " << shero::g_hook_enable 
            << ", current coroutine is main coroutine "
            << shero::Coroutine::IsMainCoroutine();
        return write_hook(fd, buf, count);
    }
    LOG_DEBUG << "write hook start";

    shero::Channel::ptr channel =
         shero::ChannelMgr::GetInstance()->getChannel(fd);
    channel->setNonBlock();

    ssize_t rt = write_hook(fd, buf, count);
    if(rt > 0) {
        return rt;
    }

    shero::Coroutine *cor = shero::Coroutine::GetCurCoroutine();
    channel->setCoroutine(cor);
    channel->addListenEvents(shero::IOEvent::WRITE);
    channel->setWriteCallback([cor]() {
        LOG_DEBUG << "write resume";
        shero::Coroutine::Resume(cor);
    });

    LOG_DEBUG << "write hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Yield";
    shero::Coroutine::Yield();
    LOG_DEBUG << "write hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Resume";

    channel->delListenEvents(shero::IOEvent::WRITE);
    channel->clearCoroutine();

    return write_hook(fd, buf, count);
}

int socket(int domain, int type, int protocol) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        LOG_DEBUG << "[socket] g_hook_enable is " << shero::g_hook_enable 
            << ", current coroutine is main coroutine "
            << shero::Coroutine::IsMainCoroutine();
        return socket_hook(domain, type, protocol);
    }
    LOG_DEBUG << "socket hook start";

    int32_t fd = socket_hook(AF_INET, SOCK_STREAM, 0);
    shero::ChannelMgr::GetInstance()->getChannel(fd);
    return fd;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        LOG_DEBUG << "[accept] g_hook_enable is " << shero::g_hook_enable 
            << ", current coroutine is main coroutine "
            << shero::Coroutine::IsMainCoroutine();
        return accept_hook(sockfd, addr, addrlen);
    }
    LOG_DEBUG << "accept hook start";

    shero::Channel::ptr channel =
         shero::ChannelMgr::GetInstance()->getChannel(sockfd);
    channel->setNonBlock();

    int32_t connfd = accept_hook(sockfd, addr, addrlen);
    if(connfd > 0) {
        return connfd;
    }

    shero::Coroutine *cor = shero::Coroutine::GetCurCoroutine();
    channel->setCoroutine(cor);
    channel->addListenEvents(shero::IOEvent::READ);
    channel->setReadCallback([cor]() {
        LOG_DEBUG << "accept resume";
        shero::Coroutine::Resume(cor);
    });

    LOG_DEBUG << "accept hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Yield";
    shero::Coroutine::Yield();
    LOG_DEBUG << "accept hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Resume";

    channel->delListenEvents(shero::IOEvent::READ);
    channel->clearCoroutine();

    return accept_hook(sockfd, addr, addrlen);
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        LOG_DEBUG << "[connect] g_hook_enable is " << shero::g_hook_enable 
            << ", current coroutine is main coroutine "
            << shero::Coroutine::IsMainCoroutine();
        return connect_hook(sockfd, addr, addrlen);
    }
    LOG_DEBUG << "connect hook start";

    shero::Channel::ptr channel =
         shero::ChannelMgr::GetInstance()->getChannel(sockfd);
    channel->setNonBlock();

    int32_t rt = connect_hook(sockfd, addr, addrlen);
    if(rt == 0) {
        return rt;
    } else if(errno != EINPROGRESS) {
        return rt;
    }

    shero::Coroutine *co = shero::Coroutine::GetCurCoroutine();
    channel->setCoroutine(co);
    channel->addListenEvents(shero::IOEvent::WRITE);
    channel->setWriteCallback([co]() {
        LOG_DEBUG << "connect resume";
        shero::Coroutine::Resume(co);
    });

    shero::EventLoop *loop = shero::EventLoop::GetEventLoop();
    shero::Timer::ptr timer = std::make_shared<shero::Timer>(loop);
    shero::Coroutine *cor = shero::Coroutine::GetCurCoroutine();
    bool is_timeout = false;
    timer->addTimer(60 * 1000, [&is_timeout, cor]() {
        LOG_DEBUG << "connect timeout";
        is_timeout = true;
        shero::Coroutine::Resume(cor);
    }, false);

    LOG_DEBUG << "connect hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Yield";
    shero::Coroutine::Yield();
    LOG_DEBUG << "connect hook, coroutine[" 
        << shero::Coroutine::GetCurCoroutine() << "] Resume";

    channel->delListenEvents(shero::IOEvent::WRITE);
    channel->clearCoroutine();

    rt = connect_hook(sockfd, addr, addrlen);
    if((rt < 0 && errno == EISCONN) || rt == 0) {
        return rt;
    }

    if(is_timeout) {
        LOG_ERROR << "connect timeout";
        errno = ETIMEDOUT;
    }
    return -1;
}

unsigned int sleep(unsigned int seconds) {
    if(!shero::g_hook_enable || shero::Coroutine::IsMainCoroutine()) {
        return sleep_hook(seconds);
    }
    LOG_DEBUG << "sleep hook start";

    shero::EventLoop *loop = shero::EventLoop::GetEventLoop();
    shero::Timer::ptr timer = std::make_shared<shero::Timer>(loop);
    shero::Coroutine *cor = shero::Coroutine::GetCurCoroutine();

    bool is_timeout = false;
    timer->addTimer(seconds * 1000, [&is_timeout, cor]() {
        is_timeout = true;
        shero::Coroutine::Resume(cor);
    }, false);

    while(!is_timeout) {
        LOG_DEBUG << "sleep hook, coroutine[" 
            << shero::Coroutine::GetCurCoroutine() << "] Yield";
        shero::Coroutine::Yield();
        LOG_DEBUG << "sleep hook, coroutine[" 
            << shero::Coroutine::GetCurCoroutine() << "] Resume";
    }

    return 0;
}

}   // extern "C"