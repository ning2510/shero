#ifndef __SHERO_UTIL_H
#define __SHERO_UTIL_H

#include <time.h>
#include <string>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>

namespace shero {

pid_t GetPid();
pid_t GetThreadId();

// 毫秒 ms
uint64_t GetCurrentMS();
// 微秒 µs
uint64_t GetCurrentUS();

std::string Date2Str(time_t ts = time(0), const std::string &format = "%Y%m%d");
std::string Time2Str(time_t ts = time(0), const std::string &format = "%Y-%m-%d %H:%M:%S");

std::string SHA1sum(const std::string &data);
std::string SHA1sum(const void *data, size_t len);
std::string Encodebase64(const std::string &data);
std::string Encodebase64(const void *data, size_t len);

std::string RandomString(size_t len);

}   // namespace shero

#endif
