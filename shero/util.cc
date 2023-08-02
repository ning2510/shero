#include "shero/util.h"
#include "shero/macro.h"

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

}   // namespace shero