#ifndef __SHERO_MEMORYPOOL_H
#define __SHERO_MEMORYPOOL_H

#include "shero/base/Mutex.h"

#include <vector>
#include <memory>
#include <atomic>

namespace shero {

class Memory {
public:
    typedef std::shared_ptr<Memory> ptr;
    typedef Mutex MutexType;
    Memory(int32_t blockSize = 256 /*KB*/, int32_t blockCount = 100);
    ~Memory();

    char *getBlock();
    void releaseBlock(char *block);
    bool hasBlock(char *block);

    int32_t getTotalSize() const { return m_totalSize; }
    int32_t getBlockSize() const { return m_blockSize; }
    int32_t getBlockCount() const { return m_blockCount; }
    int32_t getUseCount() const { return m_useCount; }

    char *getStart() const { return m_start; }
    char *getEnd() const { return m_end; }

private:
    int32_t m_totalSize;
    int32_t m_blockSize;
    int32_t m_blockCount;

    // [m_start, m_end]
    char *m_start;
    char *m_end;
    MutexType m_mutex;

    std::atomic_int32_t m_useCount;
    std::vector<bool> m_block;
};

}   // namespace shero

#endif
