#ifndef __SHERO_ENDIAN_H
#define __SHERO_ENDIAN_H

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <type_traits>

#define SHERO_LITTLE_ENDIAN 1
#define SHERO_BIG_ENDIAN 2

namespace shero {

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define SHERO_BYTE_ORDER SHERO_BIG_ENDIAN
#else
#define SHERO_BYTE_ORDER SHERO_LITTLE_ENDIAN
#endif

#if SHERO_BYTE_ORDER == SHERO_BIG_ENDIAN
template<class T>
T byteswapOnLittleEndian(T v) {
    return v;
}

template<class T>
T byteswapOnBigEndian(T v) {
    return byteswap(v);
}

#else SHERO_BYTE_ORDER == SHERO_LITTLE_ENDIAN
template<class T>
T byteswapOnLittleEndian(T v) {
    return byteswap(v);
}

template<class T>
T byteswapOnBigEndian(T v) {
    return v;
}

#endif

}   // namespace shero

#endif
