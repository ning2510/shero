#ifndef __SHERO_LOG_H
#define __SHERO_LOG_H

#include <memory>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>
#include <condition_variable>

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
    LogEvent(int32_t line, pid_t m_pid, pid_t m_tid, int32_t m_corid, 
             uint64_t m_time, const char *m_file, LogLevel::Level m_level);

    int32_t getLine() const { return m_line; }
    pid_t getPid() const { return m_pid; }
    pid_t getTid() const { return m_tid; }
    int32_t getCorid() const { return m_corid; }
    uint64_t getTime() const { return m_time; }
    const char *getFile() const { return m_file; }
    LogLevel::Level getLevel() const { return m_level; }
    std::stringstream &getSS() { return m_ss; }

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
    LogEventWrap();
    ~LogEventWrap();

    LogEvent::ptr getEvent();
    std::stringstream &getSS();

private:
    LogEvent::ptr m_event;
};

class Logger {
public:

};

}   // namespace shero
    
#endif
