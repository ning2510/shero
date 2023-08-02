#include "shero/config.h"

namespace shero {

void Config::LoadYaml(const std::string &filePath) {
    YAML::Node config;
    try {
        config = YAML::LoadFile(filePath);
    } catch (...) {
        LOG_ERROR << "error loading file, yaml file error or not exist.";
        return;
    }

    m_filePath = filePath;
    m_maxSize = config["log"]["log_max_size"].as<int32_t>();
    m_interval = config["log"]["log_sync_interval"].as<int32_t>();
    m_mode = LogMode::String2Mode(config["log"]["log_mode"].as<std::string>());
    m_level = LogLevel::String2Level(config["log"]["log_level"].as<std::string>());

    shero::LoggerManager::setDefaultLogger(false);

    LOG_INFO << "log_mode: " << (int32_t)m_mode << "\n"
        << "log_max_size: " << m_maxSize << "\n"
        << "log_sync_interval: " << m_interval << "\n"
        << "log_level: " << m_level << "\n"
        << "log_file_path: " << m_filePath << "\n";
}

void Config::LoadXml(const std::string &filePath) {
    TiXmlDocument *xmlFile = new TiXmlDocument();
    try {
        xmlFile->LoadFile("config.xml");
    } catch(...) {
        LOG_ERROR << "error loading file, xml file error or not exist.";
    }

    m_filePath = filePath;
    TiXmlElement *root = xmlFile->RootElement();
    TiXmlElement *logNode = root->FirstChildElement("log");
    TiXmlElement *node = logNode->FirstChildElement("log_path");
    if(!node || !node->GetText()) {
        delete xmlFile;
        LOG_ERROR << "error loading file, log_path field error or not exist.";
    }
    m_filePath = node->GetText();

    node = logNode->FirstChildElement("log_max_size");
    if(!node || !node->GetText()) {
        delete xmlFile;
        LOG_ERROR << "error loading file, log_max_size field error or not exist.";
    }
    m_maxSize = atoi(node->GetText());

    node = logNode->FirstChildElement("log_level");
    if(!node || !node->GetText()) {
        delete xmlFile;
        LOG_ERROR << "error loading file, log_level field error or not exist.";
    }
    std::string logLevel = node->GetText();

    node = logNode->FirstChildElement("log_sync_interval");
    if(!node || !node->GetText()) {
        delete xmlFile;
        LOG_ERROR << "error loading file, log_sync_interval field error or not exist.";
    }
    m_interval = atoi(node->GetText());

    shero::LoggerManager::setDefaultLogger(false);

    LOG_INFO << "log_mode: " << (int32_t)m_mode << "\n"
        << "log_max_size: " << m_maxSize << "\n"
        << "log_sync_interval: " << m_interval << "\n"
        << "log_level: " << m_level << "\n"
        << "log_file_path: " << m_filePath << "\n";
}

}   // namespace shero