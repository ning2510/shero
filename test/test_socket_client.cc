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

    rt = socket->connect(addr);
    assert(rt);

    while(1) {
        std::string p = "Hello";
        ssize_t rt = write(socket->getFd(), p.c_str(), sizeof(p));
        std::cout << "rt = " << rt << " write = " << p << std::endl;

        char buf[128] = {0};
        ssize_t rt2 = read(socket->getFd(), buf, sizeof(buf));
        std::cout << "rt = " << rt2 << " recv = " << buf << std::endl;
        if(rt && rt2) break;
    }

    return 0;
}