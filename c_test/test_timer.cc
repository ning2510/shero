#include <iostream>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

int main(int argc, char **argv) {
    int32_t fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    std::cout << "fd = " << fd << std::endl;

    struct itimerspec its;
    bzero(&its, sizeof(its));

    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;

    int rt = timerfd_settime(fd, 0, &its, nullptr);
    std::cout << "rt = " << rt << std::endl;
    assert(rt == 0);

    int epfd = epoll_create(1);
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    rt = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    std::cout << "rt = " << rt << std::endl;
    assert(rt == 0);

    struct epoll_event events[10];
    
    
        int nfds = epoll_wait(epfd, events, 10, -1);
        std::cout << "nfds = " << nfds << std::endl;
        for(int i = 0; i < nfds; i++) {
            auto it = events[i];
            if(it.data.fd == fd) {
                std::cout << "timer trigger" << std::endl;
                uint64_t exp;
                int s = read(fd, &exp, sizeof(uint64_t));
                std::cout << s << ' ' << exp << std::endl;
                break;
            } else {
                std::cout << "other fd trigger" << std::endl;
            }
        }
    

    rt = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
    std::cout << rt << std::endl;

    return 0;
}