#include "shero/base/Buffer.h"

#include <iostream>

void test_int(shero::Buffer::ptr buffer) {
std::cout << "=============== write [int] test start ==============\n";
#define XX(type, val, func) { \
        type tmp = val; \
        buffer->func(tmp); \
        std::cout << #type << " - " << (int64_t)tmp << std::endl; \
    }

    XX(int8_t, 1, writeInt8);
    XX(uint8_t, 2, writeUint8);
    XX(int16_t, 3, writeInt16);
    XX(uint16_t, 4, writeUint16);
    XX(int32_t, 5, writeInt32);
    XX(uint32_t, 6, writeUint32);
    XX(int64_t, 7, writeInt64);
    XX(uint64_t, 8, writeUint64);

#undef XX

    std::string buf = "Hello";
    buffer->writeLen(buf.c_str(), buf.size());
    std::cout << "send buf = " << buf << ", size = " << buf.size() << std::endl;
std::cout << "=============== write [int] test end ==============\n\n";

std::cout << "=============== read [int] test start ==============\n";
#define XX(type, func) { \
        type tmp = buffer->func(); \
        std::cout << #func << " - " << (int64_t)tmp << std::endl; \
    }

    XX(int8_t, readInt8)
    XX(uint8_t, readUint8)
    XX(int16_t, readInt16)
    XX(uint16_t, readUint16)
    XX(int32_t, readInt32)
    XX(uint32_t, readUint32)
    XX(int64_t, readInt64)
    XX(uint64_t, readUint64)
#undef XX

    char buf2[128] = {0};
    size_t n = buffer->readLen(buf2, 5);
    std::cout << "recv buf = " << buf2 << ", size = " << n << std::endl;
std::cout << "=============== read [int] test start ==============\n\n";
}


void test_Varint(shero::Buffer::ptr buffer) {
std::cout << "=============== write [Varint] test start ==============\n";
#define XX(type, val, func) { \
        type tmp = val; \
        buffer->func(tmp); \
        std::cout << #type << " - " << (int64_t)tmp << std::endl; \
    }

    XX(int32_t, 100, writeVarInt32);
    XX(uint32_t, 200, writeVarUint32);
    XX(int64_t, 300, writeVarInt64);
    XX(uint64_t, 400, writeVarUint64);

#undef XX
std::cout << "=============== write [Varint] test end ==============\n\n";

std::cout << "=============== read [Varint] test start ==============\n";
#define XX(type, func) { \
        type tmp = buffer->func(); \
        std::cout << #func << " - " << (int64_t)tmp << std::endl; \
    }

    XX(int32_t, readVarInt32)
    XX(uint32_t, readVarUint32)
    XX(int64_t, readVarInt64)
    XX(uint64_t, readVarUint64)
#undef XX
std::cout << "=============== read [Varint] test start ==============\n";
}

int main(int argc, char **argv) {
    shero::Buffer::ptr buffer(new shero::Buffer); 

    test_int(buffer);
    test_Varint(buffer);

    return 0;
}
