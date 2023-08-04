#include "shero/base/Config.h"

int main(int argc, char **argv) {
    if(argc > 1) {
        std::string filePath = argv[1];
        shero::Config config;
        config.LoadConf(filePath);
    }

    LOG_DEBUG << "LOG " << "DEBUG";
    LOG_INFO << "LOG " << "INFO";
    LOG_WARN << "LOG " << "WARN";
    LOG_ERROR << "LOG " << "ERROR";
    LOG_FATAL << "LOG " << "FATAL";

    return 0;
}