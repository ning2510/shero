#include "shero/Log.h"
#include "shero/Util.h"

#include <unistd.h>
#include <iostream>

// [2023-04-10 16:34:39.997879]	[DEBUG]	[15546]	[15549]	[2]	[/home/hadoop/tinyrpc/testcases/test_http_server.cc:168]	
// [82450441314661320119]	[QPSHttpServlet]	QPSHttpServlet Echo Success!! Your id is,1


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