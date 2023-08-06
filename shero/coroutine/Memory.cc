#include "shero/base/Log.h"
#include "shero/coroutine/Memory.h"

namespace shero {

Memory::Memory(int32_t blockSize /*= 256 KB*/, int32_t blockCount /*= 100*/)
    : m_totalSize(blockSize * blockCount),
      m_blockSize(blockSize),
      m_blockCount(blockCount),
      m_start(nullptr),
      m_end(nullptr),
      m_useCount(0) {
    
    m_start = new char[m_totalSize];
    m_end = m_start + m_totalSize - 1;
    m_block.resize(m_blockCount, false);
}

Memory::~Memory() {
    if(m_start) {
        delete[] m_start;
        m_start = m_end = nullptr;
    }
}

char *Memory::getBlock() {
    MutexType::Lock lock(m_mutex);
    for(int32_t i = 0; i < m_blockCount; ++i) {
        if(!m_block[i]) {
            m_block[i] = true;
            ++m_useCount;
            return m_start + i * m_blockSize;
        }
    }
    lock.unlock();

    LOG_WARN << "not enough memory in memory pool";
    return nullptr;
}

void Memory::releaseBlock(char *block) {
    if(!hasBlock(block)) {
        LOG_ERROR << "invalid block";
        return ;
    }

    int32_t index = (block - m_start) / m_blockSize;
    MutexType::Lock lock(m_mutex);
    if(!m_block[index]) {
        LOG_ERROR << "release block error, the " << index
            << " block of memory has not been used";
        return ;
    }
    
    m_block[index] = false;
    lock.unlock();

    --m_useCount;
}

bool Memory::hasBlock(char *block) {
    return block >= m_start && block <= m_end;
}

}   // namespace shero