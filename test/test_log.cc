#include "shero/base/Log.h"
#include "shero/base/Util.h"

#include <unistd.h>
#include <iostream>

int main(int argc, char **argv) {
    // shero::LoggerManager::setDefaultLogger(false);
    std::cout << "id = " << shero::GetThreadId() << std::endl;
    usleep(100);

    LOG_DEBUG << "LOG_DEBUG";
    LOG_INFO << "LOG_INFO";
    LOG_WARN << "LOG_WARN";

    shero::LoggerMgr::GetInstance()->reset();

    LOG_ERROR << "LOG_ERROR";
    LOG_FATAL << "LOG_FATAL";

    return 0;
}