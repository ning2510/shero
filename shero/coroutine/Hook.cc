#include "shero/base/Log.h"
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
    if(shero::g_hook_enable) {
        return read_hook(fd, buf, count);
    }

    // TODO
    std::cout << "read hook" << std::endl;
    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
    if(shero::g_hook_enable) {
        return write_hook(fd, buf, count);
    }

    // TODO
    std::cout << "write hook" << std::endl;
    return 0;
}

int socket(int domain, int type, int protocol) {
    if(!shero::g_hook_enable) {
        return socket_hook(domain, type, protocol);
    }

    // TODO
    std::cout << "socket hook" << std::endl;
    return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if(!shero::g_hook_enable) {
        return accept_hook(sockfd, addr, addrlen);
    }

    // TODO
    std::cout << "accept hook" << std::endl;
    return 0;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if(!shero::g_hook_enable) {
        return connect_hook(sockfd, addr, addrlen);
    }

    // TODO
    std::cout << "connect hook" << std::endl;
    return 0;
}

unsigned int sleep(unsigned int seconds) {
    if(!shero::g_hook_enable) {
        return sleep_hook(seconds);
    }

    // TODO
    std::cout << "sleep hook" << std::endl;
    return sleep_hook(2);
}

}   // extern "C"