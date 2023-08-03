#include "shero/log.h"
#include "shero/util.h"

#include <time.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

namespace shero {

// LogLevel
const std::string LogLevel::Level2String(LogLevel::Level l) {
    switch(l) {
#define XX(level) \
        case LogLevel::Level::level: \
            return #level;

        XX(UNKNOW);
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
    }
}

LogLevel::Level LogLevel::String2Level(const std::string &s) {
#define XX(level, str) \
    if(s == #str) { \
        return LogLevel::Level::level; \
    }

    XX(UNKNOW, unknow);
    XX(UNKNOW, UNKNOW);

    XX(DEBUG, debug);
    XX(DEBUG, DEBUG);
    
    XX(INFO, info);
    XX(INFO, INFO);

    XX(WARN, warn);
    XX(WARN, WARN);

    XX(ERROR, error);
    XX(ERROR, ERROR);

    XX(FATAL, fatal);
    XX(FATAL, FATAL);
#undef XX

    return LogLevel::Level::UNKNOW;
}

// LogMode
const std::string LogMode::Mode2String(LogMode::Mode m) {
    switch(m) {
#define XX(mode) \
        case LogMode::Mode::mode: \
            return #mode;

        XX(STDOUT);
        XX(FILE);
        XX(BOTH);
#undef XX
        default:
            return "UNKNOW";
    }
}

LogMode::Mode LogMode::String2Mode(const std::string &s) {
#define XX(mode, str) \
    if(s == #str) { \
        return LogMode::Mode::mode; \
    }
    XX(STDOUT, STDOUT);
    XX(FILE, FILE);
    XX(BOTH, BOTH);
#undef XX

    return LogMode::Mode::UNKNOW;
}

// LogEvent
LogEvent::LogEvent(int32_t line, const char *file, LogLevel::Level level)
    : m_line(line),
      m_file(file),
      m_level(level) {
}

void LogEvent::log() {
    m_ss << "\n";
    SHERO_LOGGER_ROOT->log(m_ss.str());
}

std::stringstream &LogEvent::getSS() {
    std::string buf = Time2Str();

    m_ss << "[" << buf << "]\t"
         << "[" << LogLevel::Level2String(m_level) << "]\t"
         << "[" << GetPid() << "]\t"
         << "[" << GetThreadId() << "]\t"
         << "[" << 0 << "]\t"
         << "[" << m_file << ":" << m_line << "]\t";

    return m_ss;
}

// AsyncLogger
AsyncLogger::AsyncLogger(LogMode::Mode mode, const char *filePath, 
                int32_t maxSize, int32_t interval)
    : m_filePath(filePath),
      m_mode(mode),
      m_maxSize(maxSize),
      m_interval(interval),
      m_no(0),
      m_date(""),
      m_thread(0),
      m_stop(true),
      m_reopen(true),
      m_file(nullptr)  {
    int rt = sem_init(&m_sem, 0, 0);
    assert(rt == 0);
    rt = pthread_cond_init(&m_cond, nullptr);
    assert(rt == 0);

    rt = pthread_create(&m_thread, nullptr, &AsyncLogger::mainLoop, this);
    assert(rt == 0);
    rt = sem_wait(&m_sem);
    assert(rt == 0);
}

AsyncLogger::~AsyncLogger() {
    stop();
    sem_destroy(&m_sem);
    pthread_cond_destroy(&m_cond);
    if(m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
    // std::cout << "~AsyncLogger " << m_queue.size() << std::endl;
}

void AsyncLogger::stop() {
    if(!m_stop) { 
        m_stop = true;
        pthread_cond_signal(&m_cond);
    }
    join();
}

void AsyncLogger::join() {
    pthread_join(m_thread, nullptr);
}

void AsyncLogger::push(std::vector<std::string> &buffer) {
    std::vector<std::string> tmp;
    tmp.swap(buffer);

    MutexType::Lock lock(m_mutex);
    m_queue.push(tmp);
    lock.unlock();

    pthread_cond_signal(&m_cond);
}

void *AsyncLogger::mainLoop(void *arg) {
    AsyncLogger *logger = reinterpret_cast<AsyncLogger *>(arg);
    logger->m_stop = false;
    int rt = sem_post(&logger->m_sem);
    assert(rt == 0);

    while(1) {
        MutexType::Lock lock(logger->m_mutex);
        while(logger->m_queue.empty() && !logger->m_stop) {
            pthread_cond_wait(&logger->m_cond, logger->m_mutex.getMutex());
        }
        bool isStop = logger->m_stop;
        if(isStop && logger->m_queue.empty()) {
            lock.unlock();
            break;
        }
        
        std::vector<std::string> msg;
        msg.swap(logger->m_queue.front());
        logger->m_queue.pop();
        lock.unlock();
    
        if(logger->m_mode != LogMode::Mode::FILE) {
            for(auto &i : msg) {
                std::cout << i;
            }
        }

        if(logger->m_mode != LogMode::Mode::STDOUT) {
            std::string date = Date2Str();
            if(date != logger->m_date) {
                logger->m_no = 0;
                logger->m_date = date;
                logger->m_reopen = true;
                if(logger->m_file) {
                    fclose(logger->m_file);
                }
            }

            std::stringstream ss;
            ss << logger->m_filePath << logger->m_date << "_" << logger->m_no << ".log";

            if(logger->m_reopen || !logger->m_file) {
                logger->m_file = fopen(ss.str().c_str(), "a");
                assert(logger->m_file);
                logger->m_reopen = false;
            }

            if(ftell(logger->m_file) > logger->m_maxSize) {
                fclose(logger->m_file);

                logger->m_no++;
                std::stringstream ss2;
                ss2 << logger->m_filePath << logger->m_date << "_" << logger->m_no << ".log";

                logger->m_file = fopen(ss2.str().c_str(), "a");
                assert(logger->m_file);
                logger->m_reopen = false;
            }


            for(auto &i : msg) {
                if(!i.empty()) {
                    fwrite(i.c_str(), 1, i.size(), logger->m_file);
                }
            }
            fflush(logger->m_file);
        }
    }

    if(logger->m_file) {
        fclose(logger->m_file);
        logger->m_file = nullptr;
    }

    return nullptr;
}

// Logger
Logger::Logger(LogMode::Mode mode, const char *filePath, 
        int32_t maxSize, int32_t interval, LogLevel::Level level)
    : m_level(level) {
    m_asyncLogger = std::make_shared<AsyncLogger>(mode, filePath, maxSize, interval);
}

Logger::~Logger() {
    m_asyncLogger->stop();
    // std::cout << "~Logger\n";
}

void Logger::log(const std::string &msg) {
    MutexType::Lock lock(m_mutex);
    m_buffer.push_back(msg);
    lock.unlock();

    m_asyncLogger->push(m_buffer);
}

bool LoggerManager::m_isDefault = true;

}   // namespace shero
