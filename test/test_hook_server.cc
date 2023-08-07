#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/Socket.h"
#include "shero/net/Address.h"
#include "shero/net/EventLoop.h"
#include "shero/coroutine/Hook.h"
#include "shero/coroutine/Coroutine.h"
#include "shero/coroutine/CoroutinePool.h"

#include <unistd.h>
#include <assert.h>
#include <iostream>

void corCallback() {
    uint16_t port = 6666;
    shero::Address::ptr addr = std::make_shared<shero::Address>(port);

    shero::Socket::ptr socket = shero::Socket::CreateTCP();
    bool rt = socket->init();
    assert(rt);
    rt = socket->bind(addr);
    assert(rt);

    rt = socket->listen();
    assert(rt);

    shero::Address::ptr peerAddr(new shero::Address);
    int32_t connfd = socket->accept(peerAddr);
    assert(connfd >= 0);
    LOG_DEBUG << "accept fd = " << connfd;

    while(1) {
        char buf[128] = {0};
        ssize_t rt = read(connfd, buf, sizeof(buf));
        LOG_DEBUG << "rt = " << rt << " recv = " << buf;

        std::string p = "Hello";
        ssize_t rt2 = write(connfd, p.c_str(), sizeof(p));
        LOG_DEBUG << "rt = " << rt2 << " send = " << buf;
        if(rt && rt2) break;
    }

    shero::EventLoop::GetEventLoop()->quit();
}

int main() {
    LOG_DEBUG << "server tid = " << shero::GetThreadId();
    shero::set_hook_enable(true);
    
    shero::EventLoop *loop = shero::EventLoop::GetEventLoop();

    shero::Coroutine::ptr cor = 
        shero::CoroutinePool::GetCoroutinePool()->getCoroutineInstance();

    cor->setCallback(corCallback);

    LOG_DEBUG << "coroutine start, cor = " << cor.get();
    shero::Coroutine::Resume(cor.get());

    loop->loop();
    LOG_DEBUG << "coroutine end, cor = " << cor.get();
    
    return 0;
}