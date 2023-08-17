#ifndef __SHERO_LOG_H
#define __SHERO_LOG_H

#include "shero/base/Mutex.h"
#include "shero/base/Thread.h"
#include "shero/base/Singleton.h"

#include <iostream>
#include <queue>
#include <vector>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <condition_variable>

#define SHERO_LOGGER_ROOT shero::LoggerMgr::GetInstance()->getLogger()
#define SHERO_LOGGER_CONF shero::LoggerMgr::GetInstance()

#define LOG_DEBUG \
    if(SHERO_LOGGER_ROOT && shero::LogLevel::DEBUG >= SHERO_LOGGER_ROOT->getLevel()) \
        shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
                __LINE__, __FILE__, shero::LogLevel::Level::DEBUG)).getSS()

#define LOG_INFO \
    if(SHERO_LOGGER_ROOT && shero::LogLevel::INFO >= SHERO_LOGGER_ROOT->getLevel()) \
        shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
                __LINE__, __FILE__, shero::LogLevel::Level::INFO)).getSS()

#define LOG_WARN \
    if(SHERO_LOGGER_ROOT && shero::LogLevel::WARN >= SHERO_LOGGER_ROOT->getLevel()) \
        shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
                __LINE__, __FILE__, shero::LogLevel::Level::WARN)).getSS()

#define LOG_ERROR \
    if(SHERO_LOGGER_ROOT && shero::LogLevel::ERROR >= SHERO_LOGGER_ROOT->getLevel()) \
        shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
                __LINE__, __FILE__, shero::LogLevel::Level::ERROR)).getSS()

#define LOG_FATAL \
    if(SHERO_LOGGER_ROOT && shero::LogLevel::FATAL >= SHERO_LOGGER_ROOT->getLevel()) \
        shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
                __LINE__, __FILE__, shero::LogLevel::Level::FATAL)).getSS()

namespace shero {

class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static const std::string Level2String(LogLevel::Level level);
    static LogLevel::Level String2Level(const std::string &str);
};

class LogMode {
public:
    enum Mode {
        UNKNOW = 0,
        STDOUT = 1,
        FILE = 2,
        BOTH = 3
    };

    static const std::string Mode2String(LogMode::Mode mode);
    static LogMode::Mode String2Mode(const std::string &str);
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(int32_t line, const char *file, LogLevel::Level level);
    std::stringstream &getSS();
    void log();

    int32_t getLine() const { return m_line; }
    pid_t getPid() const { return m_pid; }
    pid_t getTid() const { return m_tid; }
    int32_t getCorid() const { return m_corid; }
    uint64_t getTime() const { return m_time; }
    const char *getFile() const { return m_file; }
    LogLevel::Level getLevel() const { return m_level; }

private:
    // 行号
    int32_t m_line;
    // 进程 id
    pid_t m_pid;
    // 线程 id
    pid_t m_tid;
    // 协程 id
    int32_t m_corid;
    // 时间戳
    uint64_t m_time;
    // 文件名
    const char *m_file;
    // 日志级别
    LogLevel::Level m_level;
    // 日志内容
    std::stringstream m_ss;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr event) : m_event(event) {}
    ~LogEventWrap() {
        m_event->log();
        if(m_event->getLevel() == LogLevel::Level::FATAL) {
            exit(0);
        }
    }

    std::stringstream &getSS() { return m_event->getSS(); }

private:
    LogEvent::ptr m_event;
};

class AsyncLogger {
public:
    typedef std::shared_ptr<AsyncLogger> ptr;
    typedef Mutex MutexType;

    AsyncLogger(LogMode::Mode mode, const char *filePath, int32_t maxSize, int64_t interval);
    ~AsyncLogger();

    void stop();
    void join();
    void push(std::vector<std::string> &buffer);

    static void *mainLoop(void *arg);

    LogMode::Mode getMode() const { return m_mode; }
    int32_t getMaxSize() const { return m_maxSize; }
    int64_t getInterval() const { return m_interval; }
    int32_t getNo() const { return m_no; }
    const char *getFilePath() const { return m_filePath; }

    bool isStop() const { return m_stop; }
    bool isReOpen() const { return m_reopen; }

public:
    std::queue<std::vector<std::string> > m_queue;

private:
    const char *m_filePath;
    // 0: stdout, 1: file, 2: both
    LogMode::Mode m_mode;
    int32_t m_maxSize;
    int64_t m_interval;
    int32_t m_no;
    std::string m_date;
    pthread_t m_thread;

    bool m_stop;
    bool m_reopen;

    FILE *m_file;

    MutexType m_mutex;
    sem_t m_sem;
    pthread_cond_t m_cond;
};

class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Mutex MutexType;
    // 5MB, 500ms
    Logger(LogMode::Mode mode, const char *filePath, 
        int32_t maxSize, int64_t interval, LogLevel::Level level);
    ~Logger();

    void log(std::string msg);

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level v) { m_level = v; }

private:
    MutexType m_mutex;
    LogLevel::Level m_level;
    AsyncLogger::ptr m_asyncLogger;

    std::vector<std::string> m_buffer;
};

class LoggerManager {
public:
    LoggerManager() {
        if(m_isDefault) {
            reset();
        }
    }

    void reset(LogMode::Mode mode = LogMode::Mode::FILE, const char *filePath = "./", 
            int32_t maxSize = 5 * 1024 * 1024, int64_t interval = 500, 
            LogLevel::Level level = LogLevel::Level::DEBUG) {
#ifdef BENCH_MARK
        std::cout << "Bench mark\n";
        g_logger.reset(new Logger(mode, filePath, maxSize, interval, LogLevel::Level::ERROR));
#else
        std::cout << "not Bench mark\n";
        g_logger.reset(new Logger(mode, filePath, maxSize, interval, level));
#endif
    }

    Logger *getLogger() const { return g_logger.get(); }
    static void setDefaultLogger(bool v) { m_isDefault = v; }

private:
    Logger::ptr g_logger;
    static bool m_isDefault;
};

typedef Singleton<LoggerManager> LoggerMgr;

}   // namespace shero
    
#endif
