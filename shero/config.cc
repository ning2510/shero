#include "shero/config.h"

namespace shero {

Config::Config() 
    : m_init(false),
      m_mode(LogMode::Mode::FILE),
      m_maxSize(5 * 1024 * 1024),
      m_interval(500),
      m_level(LogLevel::Level::DEBUG),
      m_filePath("./"),
      m_xmlFile(nullptr) {
}

Config::~Config() {
    if(m_xmlFile) {
        delete m_xmlFile;
        m_xmlFile = nullptr;
    }
}

void Config::LoadConf(std::string filePath) {
    auto it = filePath.find_last_of(".");
    if(it == filePath.npos) {
        LOG_WARN << "file type error, use default log config";
        return ;
    }

    std::string suffix = filePath.substr(it + 1);
    bool rt = false;
    if(suffix == "yaml") {
        rt = LoadYaml(filePath);
    } else if(suffix == "xml") {
        rt = LoadXml(filePath);
    } else {
        LOG_WARN << "Does not support " << suffix 
            <<" configuration files, use the default configuration";
        return ;
    }

    if(!rt) return ;

    shero::LoggerManager::setDefaultLogger(false);
    SHERO_LOGGER_CONF->reset(m_mode, m_filePath.c_str(), m_maxSize, m_interval, m_level);
    m_init = true;

    // std::cout << "log_mode: " << (int32_t)m_mode << "\n"
    //     << "log_max_size: " << m_maxSize << "\n"
    //     << "log_sync_interval: " << m_interval << "\n"
    //     << "log_level: " << m_level << "\n"
    //     << "log_file_path: " << m_filePath << "\n";
}

bool Config::LoadYaml(const std::string &filePath) {
    YAML::Node config;
    try {
        config = YAML::LoadFile(filePath);
    } catch (...) {
        LOG_WARN << "error loading file, yaml file error or not exist, "
            "use default log config.";
        return false;
    }

    m_filePath = config["log"]["log_path"].as<std::string>();
    m_maxSize = config["log"]["log_max_size"].as<int32_t>() * 1024 * 1024;
    m_interval = config["log"]["log_sync_interval"].as<int32_t>();

    m_mode = LogMode::String2Mode(config["log"]["log_mode"].as<std::string>());
    if(m_mode == LogMode::Mode::UNKNOW) {
        LOG_WARN << "log_mode filed error, use default log config.";
        return false;
    }
    
    m_level = LogLevel::String2Level(config["log"]["log_level"].as<std::string>());
    if(m_level == LogLevel::Level::UNKNOW) {
        LOG_WARN << "log_level filed error, use default log config.";
        return false;
    }

    return true;
}

bool Config::LoadXml(const std::string &filePath) {
    m_xmlFile = new TiXmlDocument();
    try {
        m_xmlFile->LoadFile(filePath.c_str());
    } catch(...) {
        LOG_WARN << "error loading file, xml file error or not exist, "
            "use default log config.";
        return false;
    }

    TiXmlElement *root = m_xmlFile->RootElement();
    TiXmlElement *logNode = root->FirstChildElement("log");
    if(!logNode) {
        LOG_WARN << "config file doesn't have log field, use default config";
    }
    TiXmlElement *node = logNode->FirstChildElement("log_mode");
    if(!node || !node->GetText() || std::string(node->GetText()) == "UNKNOW") {
        LOG_WARN << "error loading file, log_mode field error or not exist, "
            "use default log config.";
        return false;
    }
    m_mode = LogMode::String2Mode(node->GetText());
    if(m_mode == LogMode::Mode::UNKNOW) {
        LOG_WARN << "log_mode filed error, use default log config.";
        return false;
    }

    node = logNode->FirstChildElement("log_path");
    if(!node || !node->GetText()) {
        LOG_WARN << "error loading file, log_path field error or not exist, "
            "use default log config.";
        return false;
    }
    m_filePath = node->GetText();

    node = logNode->FirstChildElement("log_max_size");
    if(!node || !node->GetText()) {
        LOG_WARN << "error loading file, log_max_size field error or not exist, "
            "use default log config.";
        return false;
    }
    m_maxSize = atoi(node->GetText()) * 1024 * 1024;

    node = logNode->FirstChildElement("log_level");
    if(!node || !node->GetText()) {
        LOG_WARN << "error loading file, log_level field error or not exist, "
            "use default log config.";
        return false;
    }
    m_level = LogLevel::String2Level(node->GetText());
    if(m_level == LogLevel::Level::UNKNOW) {
        LOG_WARN << "log_level filed error, use default log config.";
        return false;
    }
    
    node = logNode->FirstChildElement("log_sync_interval");
    if(!node || !node->GetText()) {
        LOG_WARN << "error loading file, log_sync_interval field error or not exist, "
            "use default log config.";
        return false;
    }
    m_interval = atoi(node->GetText());

    return true;
}

}   // namespace shero