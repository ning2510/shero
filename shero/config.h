#ifndef __SHERO_CONFIG_H
#define __SHERO_CONFIG_H

#include "shero/log.h"
#include "shero/mutex.h"

#include <memory>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <tinyxml/tinyxml.h>

namespace shero {

class Config {
public:
    std::shared_ptr<Config> ptr;
    Config() {}

    void LoadYaml(const std::string &filePath);
    void LoadXml(const std::string &filePath);

    LogMode::Mode getMode() const { return m_mode; }
    int32_t getMaxSize() const { return m_maxSize; }
    int32_t getInterval() const { return m_interval; }
    LogLevel::Level getLevel() const { return m_level; }
    std::string getFilePath() const { return m_filePath; }

private:
    LogMode::Mode m_mode;
    int32_t m_maxSize;
    int32_t m_interval;
    LogLevel::Level m_level;
    std::string m_filePath;

};
    
}   // namespace shero

#endif
