#ifndef __SHERO_BUFFER_H
#define __SHERO_BUFFER_H

#include "shero/base/Endian.h"

#include <vector>
#include <memory>

namespace shero {

/*
                       1024 bytes
                   ___________________
                 /                     \
+---------------+-----------------------+
| m_freePrepend |     m_initialSize     |
+---------------+-----------.-----------+
\______________/|           |
    8 bytes     |           v
                v      m_writeIndex
          m_readIndex     
*/

class Buffer {
public:
    typedef std::shared_ptr<Buffer> ptr;
    static const size_t m_freePrepend = 8;
    static const size_t m_initialSize = 1024;

    Buffer(size_t initialSize = m_initialSize)
        : m_readIndex(m_freePrepend),
          m_writeIndex(m_freePrepend),
          m_buffer(m_freePrepend + initialSize) {
    }

    // 固定长度 read
    int8_t readInt8();
    uint8_t readUint8();
    int16_t readInt16();
    uint16_t readUint16();
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();
    size_t readLen(void *buf, size_t len);

    // 固定长度 write
    void writeInt8(int8_t host8);
    void writeUint8(uint8_t host8);
    void writeInt16(int16_t host16);
    void writeUint16(uint16_t host16);
    void writeInt32(int32_t host32);
    void writeUint32(uint32_t host32);
    void writeInt64(int64_t host64);
    void writeUint64(uint64_t host64);
    void writeLen(const char *data, size_t len);
    void writeLen(const void *data, size_t len);

    // 可变长整数 read
    int32_t readVarInt32();
    uint32_t readVarUint32();
    int64_t readVarInt64();
    uint64_t readVarUint64();

    // 可变长整数 write
    void writeVarInt32(int32_t v);
    void writeVarUint32(uint32_t v);
    void writeVarInt64(int64_t v);
    void writeVarUint64(uint64_t v);
    
    size_t readFd(int32_t fd, int32_t *saveErrno);
    size_t writeFd(int32_t fd, int32_t *saveErrno);

    void ensureWriteableBytes(size_t len) {
        if(writeableBytes() < len) {
            resizeBuffer(len);
        }
    }

    void retrieve(size_t len) {
        if(readableBytes() < len) {
            retrieveAll();
        } else {
            m_readIndex += len;
        }
    }
 
    void retrieveAll() {
        m_readIndex = m_writeIndex = m_freePrepend;
    }

    std::string retrieveAsString(size_t len) {
        std::string buf(readIndex(), len);
        retrieve(len);
        return buf;
    }
    
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    size_t readableBytes() const { return m_writeIndex - m_readIndex; }
    size_t writeableBytes() const { return m_buffer.size() - m_writeIndex; }
    size_t prependableBytes() const { return m_readIndex; }

    char *readIndex() { return begin() + m_readIndex; }
    const char *readIndex() const { return begin() + m_readIndex; }
    char *writeIndex() { return begin() + m_writeIndex; }
    const char *writeIndex() const { return begin() + m_writeIndex; }

private:
    char *begin() {
        return &*m_buffer.begin();
    }

    const char *begin() const {
        return &*m_buffer.begin();
    }

    // 扩容
    void resizeBuffer(size_t len) {
        
    }

private:
    size_t m_readIndex;
    size_t m_writeIndex;
    std::vector<char> m_buffer;
};

}   // namespace shero

#endif
