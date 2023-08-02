#ifndef __SHERO_LOG_H
#define __SHERO_LOG_H

#include "shero/mutex.h"
#include "shero/thread.h"
#include "shero/singleton.h"

#include <queue>
#include <vector>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <condition_variable>

#define LOG_DEBUG \
    shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
            __LINE__, __FILE__, shero::LogLevel::Level::DEBUG)).getSS()

#define LOG_INFO \
    shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
            __LINE__, __FILE__, shero::LogLevel::Level::INFO)).getSS()

#define LOG_WARN \
    shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
            __LINE__, __FILE__, shero::LogLevel::Level::WARM)).getSS()

#define LOG_ERROR \
    shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
            __LINE__, __FILE__, shero::LogLevel::Level::ERROR)).getSS()

#define LOG_FATAL \
    shero::LogEventWrap(std::make_shared<shero::LogEvent>( \
            __LINE__, __FILE__, shero::LogLevel::Level::FATAL)).getSS()

#define SHERO_LOGGER_ROOT shero::LoggerMgr::GetInstance()->getLogger()

namespace shero {

class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARM = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static const std::string Level2String(LogLevel::Level level);
    static LogLevel::Level String2Level(const std::string &str);
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
    ~LogEventWrap() { m_event->log(); }

    std::stringstream &getSS() { return m_event->getSS(); }

private:
    LogEvent::ptr m_event;
};

class AsyncLogger {
public:
    typedef std::shared_ptr<AsyncLogger> ptr;
    typedef Mutex MutexType;

    AsyncLogger(const char *filePath, int32_t maxSize, int32_t interval);
    ~AsyncLogger();

    void stop();
    void join();
    void push(std::vector<std::string> &buffer);
    std::string getDate();

    static void *mainLoop(void *arg);

public:
    std::queue<std::vector<std::string> > m_queue;

private:
    const char *m_filePath;
    int32_t m_maxSize;
    int32_t m_interval;
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
    Logger(const char *filePath, int32_t maxSize, int32_t interval, LogLevel::Level level);
    ~Logger();

    void log(const std::string &msg);

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
        reset();
    }

    void reset(const char *filePath = "./", int32_t maxSize = 5 * 1024 * 1024, 
            int32_t interval = 500, LogLevel::Level level = LogLevel::Level::DEBUG) {
        g_logger.reset(new Logger(filePath, maxSize, interval, level));
    }

    Logger::ptr getLogger() const { return g_logger; }

private:
    Logger::ptr g_logger;
};

typedef Singleton<LoggerManager> LoggerMgr;

}   // namespace shero
    
#endif
