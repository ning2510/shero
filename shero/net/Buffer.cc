// #include "shero/base/Log.h"
#include "shero/net/Buffer.h"

#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <iostream>

namespace shero {

const char Buffer::kCRLF[] = "\r\n";

size_t Buffer::readFd(int32_t fd, int32_t *saveErrno) {
    char buf[65536] = {0};
    struct iovec vec[2];

    const size_t writable = writeableBytes();
    vec[0].iov_base = writeIndex();
    vec[0].iov_len = writable;

    vec[1].iov_base = buf;
    vec[1].iov_len = sizeof(buf);

    const int iovcnt = writable < sizeof(buf) ? 2 : 1;
    const ssize_t rt = ::readv(fd, vec, iovcnt);
    if(rt < 0) {
        *saveErrno = errno;
    } else if(rt <= (ssize_t)writable) {
        m_writeIndex += rt;
    } else {
        m_writeIndex = m_buffer.size();
        writeLen(buf, rt - writable);
    }
    return rt;
}

size_t Buffer::writeFd(int32_t fd, int32_t *saveErrno) {
    ssize_t rt = ::write(fd, readIndex(), readableBytes());
    if(rt < 0) {
        *saveErrno = errno;
    }
    return rt;
}

    // LOG_ERROR << "Buffer::read" #type "() error, the number of "
    //     "remaining readable bytes is less than " << sizeof(type);

// 固定长度 read
#define XX(type) \
    if(readableBytes() < sizeof(type)) { \
        return 0; \
    } \
    type nw; \
    readLen((void *)&nw, sizeof(type)); \
    type host = byteswapOnLittleEndian(nw); \
    return host; 

int8_t Buffer::readInt8() { XX(int8_t) }

uint8_t Buffer::readUint8() { XX(uint8_t) }

int16_t Buffer::readInt16() { XX(int16_t) }

uint16_t Buffer::readUint16() { XX(uint16_t) }

int32_t Buffer::readInt32() { XX(int32_t) }

uint32_t Buffer::readUint32() { XX(uint32_t) }

int64_t Buffer::readInt64() { XX(int64_t) }

uint64_t Buffer::readUint64() { XX(uint64_t) }
#undef XX

size_t Buffer::readLen(void *buf, size_t len) {
    memcpy(buf, readIndex(), len);
    retrieve(len);
    return len;
}

// 固定长度 write
#define XX(type, host) \
    type nw = byteswapOnLittleEndian(host); \
    writeLen(&nw, sizeof(type));

void Buffer::writeInt8(int8_t host8) { XX(int8_t, host8) }

void Buffer::writeUint8(uint8_t host8) { XX(uint8_t, host8) }

void Buffer::writeInt16(int16_t host16) { XX(int16_t, host16) }

void Buffer::writeUint16(uint16_t host16) { XX(int16_t, host16) }

void Buffer::writeInt32(int32_t host32) { XX(int32_t, host32) }

void Buffer::writeUint32(uint32_t host32) { XX(int32_t, host32) }

void Buffer::writeInt64(int64_t host64) { XX(int64_t, host64) }

void Buffer::writeUint64(uint64_t host64) { XX(uint64_t, host64) }
#undef XX

void Buffer::writeLen(const char *data, size_t len) {
    writeLen((const void *)data, len);
}

void Buffer::writeLen(const void *data, size_t len) {
    ensureWriteableBytes(len);
    memcpy(writeIndex(), data, len);
    m_writeIndex += len;
}

// 可变长整数 编解码
static uint32_t EncodeZigzag32(const int32_t &v) {
    if(v < 0) {
        return((uint32_t)(-v)) * 2 - 1;
    }
    return v * 2;
}

static uint64_t EncodeZigzag64(const int64_t &v) {
    if(v < 0) {
        return((uint64_t)(-v)) * 2 - 1;
    }
    return v * 2;
}

static int32_t DecodeZigzag32(const uint32_t &v) {
    return (v >> 1) ^ -(v & 1);
}

static int64_t DecodeZigzag64(const uint64_t &v) {
    return (v >> 1) ^ -(v & 1);
}

// 可变长整数 read
int32_t Buffer::readVarInt32() {
    return DecodeZigzag32(readVarUint32());
}

uint32_t Buffer::readVarUint32() {
    uint32_t result = 0;
    for(int i = 0; i < 32; i += 7) {
        uint8_t x = readUint8();
        if(x < 0x80) {
            result |= ((uint32_t)x) << i;
            break;
        } else {
            result |= ((uint32_t)(x & 0x7f)) << i;
        }
    }
    return result;
}

int64_t Buffer::readVarInt64() {
    return DecodeZigzag64(readVarUint64());
}

uint64_t Buffer::readVarUint64() {
    uint64_t result = 0;
    for(int i = 0; i < 64; i += 7) {
        uint8_t x = readUint8();
        if(x < 0x80) {
            result |= ((uint64_t)x) << i;
            break;
        } else {
            result |= ((uint64_t)(x & 0x7f)) << i;
        }
    }
    return result;
}

// 可变长整数 write
void Buffer::writeVarInt32(int32_t v) {
    writeVarUint32(EncodeZigzag32(v));
}

void Buffer::writeVarUint32(uint32_t v) {
    uint8_t tmp[5];
    uint8_t i = 0;
    while(v >= 0x80) {
        tmp[i++] = (v & 0x7F) | 0x80;
        v >>= 7;
    }
    tmp[i++] = v;
    writeLen(tmp, i);
}

void Buffer::writeVarInt64(int64_t v) {
    writeVarUint64(EncodeZigzag64(v));
}

void Buffer::writeVarUint64(uint64_t v) {
    uint8_t tmp[10];
    uint8_t i = 0;
    while(v >= 0x80) {
        tmp[i++] = (v & 0x7F) | 0x80;
        v >>= 7;
    }
    tmp[i++] = v;
    writeLen(tmp, i);
}


}   // namespace shero