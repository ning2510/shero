#include "shero/util.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace shero {

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

}   // namespace shero