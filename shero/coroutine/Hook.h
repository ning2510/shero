#ifndef __SHERO_HOOK_H
#define __SHERO_HOOK_H

#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace shero {

bool is_hook_enable();
void set_hook_enable(bool flag);

}   // namespace shero

extern "C" {

typedef ssize_t (* read_hook_func)(int fd, void *buf, size_t count);
extern read_hook_func read_hook;

typedef ssize_t (* readv_hook_func)(int fd, const struct iovec *iov, int iovcnt);
extern readv_hook_func readv_hook;

typedef ssize_t (* write_hook_func)(int fd, const void *buf, size_t count);
extern write_hook_func write_hook;

typedef int (* socket_hook_func)(int domain, int type, int protocol);
extern socket_hook_func socket_hook;

typedef int (* accept_hook_func)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern accept_hook_func accept_hook;

typedef int (* connect_hook_func)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_hook_func connect_hook;

typedef unsigned int (* sleep_hook_func)(unsigned int seconds);
extern sleep_hook_func sleep_hook;

}   // extern "C"

#endif
