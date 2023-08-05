#include "shero/net/Address.h"

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace shero {

Address::Address(const sockaddr_in &addr)
    : m_addr(addr) {
}

Address::Address(uint16_t port /*= 0*/, const char *address /*= "0.0.0.0"*/) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = byteswapOnLittleEndian(port);
    m_addr.sin_addr.s_addr = inet_addr(address);
}

std::string Address::toIP() {
    char buf[64] = {0};
    inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf));
    return buf;
}

std::string Address::toIpPort() {
    std::string ip = toIP();
    uint16_t port = getPort();
    char buf[128] = {0};
    sprintf(buf, "%s:%u", ip.c_str(), port);
    return buf;
}


}   // namespace shero