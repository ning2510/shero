#ifndef __SHERO_COROUTINEPOOL_H
#define __SHERO_COROUTINEPOOL_H

#include "shero/base/Mutex.h"
#include "shero/coroutine/Memory.h"
#include "shero/coroutine/Coroutine.h"

#include <memory>
#include <vector>

namespace shero {

class CoroutinePool {
public:
    typedef std::shared_ptr<CoroutinePool> ptr;
    typedef Mutex MutexType;
    ~CoroutinePool();

    void addCortineInstance();
    Coroutine::ptr getCoroutineInstance();
    void releaseCoroutine(Coroutine::ptr cor);

    static CoroutinePool *GetCoroutinePool(
            int32_t blockSize = 256 * 1024 /*256 KB*/, int32_t poolSize = 100);
private:
    CoroutinePool(int32_t blockSize, int32_t poolSize);

private:
    MutexType m_mutex;
    int32_t m_poolSize;
    int32_t m_blockSize;
    size_t m_useCount;

    std::vector<std::pair<Coroutine::ptr, bool>> m_cors;
    std::vector<Memory::ptr> m_memoryPool;
};

}   // namespace shero

#endif
