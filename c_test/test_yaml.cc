
#include <iostream>
#include "yaml-cpp/yaml.h"

using namespace std;

int main(int argc,char** argv)
{
    YAML::Node config;
    // 当文件不存在或yaml格式出错时，抛异常
    try {
        config = YAML::LoadFile("config.yaml");
    } catch (...) {
        printf("error loading file, yaml file error or not exist.\n");
        return 0;
    }

    cout << config["log"]["log_path"] << endl;
    cout << config["log"]["log_max_size"] << endl;
    cout << config["log"]["log_level"] << endl;
    cout << config["log"]["log_sync_interval"] << endl;

    return 0;
}
