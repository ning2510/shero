#include "shero/net/Socket.h"
#include "shero/net/Address.h"

#include <unistd.h>
#include <assert.h>
#include <iostream>

int main() {
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

    while(1) {
        char buf[128] = {0};
        ssize_t rt = read(connfd, buf, sizeof(buf));
        std::cout << "rt = " << rt << " recv = " << buf << std::endl;

        std::string p = "Hello";
        ssize_t rt2 = write(connfd, p.c_str(), sizeof(p));
        std::cout << "rt = " << rt2 << " send = " << buf << std::endl;
        if(rt && rt2) break;
    }

    return 0;
}