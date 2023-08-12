#include "shero/base/Log.h"
#include "shero/net/Address.h"

#include <vector>
#include <netdb.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace shero {

Address::ptr Address::LookupAddressByHost(const std::string &host, 
    int32_t family /*= AF_UNSPEC*/, int32_t type /*= 0*/, int32_t protocol /*= 0*/) {
    
    addrinfo hints, *results, *next;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    std::string node;
    const char *service = NULL;

    // 检查 ipv6address service
    if(!host.empty() && host[0] == '[') {
        const char *endipv6 = (const char *)memchr(host.c_str() + 1, ']', host.size() - 1);
        if(endipv6) {
            // TODO check out of range
            if(*(endipv6 + 1) == ':') {
                service = endipv6 + 2;
            }
            node = host.substr(1, endipv6 - host.c_str() - 1);
        }
    }

    // 检查 node service
    if(node.empty()) {
        service = (const char *)memchr(host.c_str(), ':', host.size());
        if(service) {
            if(!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {
                node = host.substr(0, service - host.c_str());
                ++service;
            }
        }
    }

    if(node.empty()) {
        node = host;
    }

    std::vector<Address::ptr> result;
    int32_t error = getaddrinfo(node.c_str(), service, &hints, &results);
    if(error) {
        LOG_ERROR << "Address::Loopup getaddress(" << host 
            << ", " << family << ", " << type << ") errnor = " << error
            << " errstr = " << strerror(error);
        return nullptr;
    }

    next = results;
    while(next) {
        sockaddr_in addr = *((sockaddr_in *)(next->ai_addr));
        result.push_back(std::make_shared<Address>(addr));
        next = next->ai_next;
    }

    freeaddrinfo(results);

    for(auto &i : result) {
        if(i) {
            return i;
        }
    }
    return nullptr;
}

Address::Address(const sockaddr_in &addr)
    : m_addr(addr) {
}

Address::Address(uint16_t port /*= 0*/, const char *address /*= "0.0.0.0"*/) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = byteswapOnLittleEndian(port);
    m_addr.sin_addr.s_addr = inet_addr(address);
}

const std::string Address::toIP() const {
    char buf[64] = {0};
    inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf));
    return buf;
}

const std::string Address::toIpPort() const {
    std::string ip = toIP();
    uint16_t port = getPort();
    char buf[128] = {0};
    sprintf(buf, "%s:%u", ip.c_str(), port);
    return buf;
}


}   // namespace shero