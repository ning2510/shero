#ifndef __SHERO_EVENTLOOPTHREADPOOL_H
#define __SHERO_EVENTLOOPTHREADPOOL_H

#include "shero/net/EventLoop.h"
#include "shero/base/Noncopyable.h"
#include "shero/net/EventLoopThread.h"

#include <memory>
#include <vector>
#include <functional>

namespace shero {

class EventLoopThreadPool : public Noncopyable {
public:
    typedef std::shared_ptr<EventLoopThreadPool> ptr;
    typedef std::function<void(EventLoop *)> ThreadInitCallback;
    EventLoopThreadPool(EventLoop *baseLoop, const std::string nameArg = "");
    ~EventLoopThreadPool();

    void start(const ThreadInitCallback &cb = nullptr);
    
    EventLoop *GetNextLoop();
    std::vector<EventLoop *> getAllLoops();

    void setThreadNums(int32_t nums) { m_numThreads = nums; }
    int32_t getThreadNums() const { return m_numThreads; }

    bool isStart() const { return m_start; }
    const std::string getName() const { return m_name; }

private:
    bool m_start;
    int32_t m_next;
    int32_t m_numThreads;
    std::string m_name;
    EventLoop *m_baseLoop;

    std::vector<EventLoop *> m_loops;
    std::vector<std::unique_ptr<EventLoopThread> > m_threads;
};

}   // namespace shero

#endif
