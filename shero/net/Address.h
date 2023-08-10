#ifndef __SHERO_ADDRESS_H
#define __SHERO_ADDRESS_H

#include "shero/base/Endian.h"

#include <memory>
#include <ostream>
#include <netinet/in.h>

namespace shero {

class Address {
public:
    typedef std::shared_ptr<Address> ptr;
    Address(const sockaddr_in &addr);
    Address(uint16_t port = 0, const char *address = "0.0.0.0");

    int32_t getFamily() const { return m_addr.sin_family; }

    const sockaddr *getAddr() const { return (sockaddr *)&m_addr; }
    sockaddr *getAddr() { return (sockaddr *)&m_addr; }
    void setAddr(const sockaddr_in &addr) { m_addr = addr; }
    socklen_t getAddrLen() const { return sizeof(m_addr); }

    uint16_t getPort() const { return byteswapOnLittleEndian(m_addr.sin_port); }
    void setPort(uint16_t v) { m_addr.sin_port = byteswapOnLittleEndian(v); }

    const std::string toIP() const;
    const std::string toIpPort() const;

private:
    sockaddr_in m_addr;
};

}   // namespace shero

#endif
