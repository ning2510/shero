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
    explicit Address(const sockaddr_in &addr);
    explicit Address(uint16_t port, const char *address = "0.0.0.0");
    explicit Address(uint16_t port = 0, uint32_t address = INADDR_ANY);

    int32_t getFamily() const { return m_addr.sin_family; }

    const sockaddr *getAddr() const { return (sockaddr *)&m_addr; }
    sockaddr *getAddr() { return (sockaddr *)&m_addr; }
    void setAddr(const sockaddr_in &addr) { m_addr = addr; }
    socklen_t getAddrLen() const { return sizeof(m_addr); }

    uint16_t getPort() const { return byteswapOnLittleEndian(m_addr.sin_port); }
    void setPort(uint16_t v) { m_addr.sin_port = byteswapOnLittleEndian(v); }

    std::string toIP();
    std::string toIpPort();

private:
    sockaddr_in m_addr;
};

}   // namespace shero

#endif
