#include "shero/Util.h"
#include "shero/Macro.h"

#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace shero {

static thread_local pid_t tid = 0;

pid_t GetPid() {
    return getpid();
}

pid_t GetThreadId() {
    if(SHERO_UNLICKLY(tid == 0)) {
        tid = syscall(SYS_gettid);
    }
    return tid;
}

// 毫秒 ms
uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

// 微秒 µs
uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

std::string Date2Str(time_t ts /*= time(0)*/, const std::string &format /*= "%Y%m%d"*/) {
    return Time2Str(ts, format);
}

std::string Time2Str(time_t ts /*= time(0)*/, const std::string &format /*= "%Y-%m-%d %H:%M:%S"*/) {
    struct tm tm;
    bzero(&tm, sizeof(tm));
    localtime_r(&ts, &tm);
    char buf[64] = {0};
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}


}   // namespace shero