#include "shero/base/Log.h"
#include "shero/base/Macro.h"
#include "shero/coroutine/CoroutinePool.h"

#include <iostream>

namespace shero {

static CoroutinePool::ptr t_coroutinePool_ptr = nullptr;
static CoroutinePool *t_coroutinePool = nullptr;

CoroutinePool *CoroutinePool::GetCoroutinePool(
        int32_t blockSize /*= 256 * 1024 B*/, int32_t poolSize /*= 100*/) {
    if(SHERO_UNLICKLY(!t_coroutinePool_ptr)) {
        t_coroutinePool_ptr.reset(new CoroutinePool(blockSize, poolSize));
        t_coroutinePool = t_coroutinePool_ptr.get();
    }
    return t_coroutinePool;
}

CoroutinePool::CoroutinePool(int32_t blockSize, int32_t poolSize)
    : m_poolSize(poolSize),
      m_blockSize(blockSize),
      m_useCount(0) {
    Coroutine::GetCurCoroutine();

    addCortineInstance();
}

CoroutinePool::~CoroutinePool() {
    for(auto &it : m_cors) {
        it.first.reset();
    }
    for(auto &it : m_memoryPool) {
        it.reset();
    }

    // std::cout << "~CoroutinePool" << std::endl;
}

void CoroutinePool::addCortineInstance() {
    int size = m_memoryPool.size();
    int total = size * m_poolSize;
    m_memoryPool.push_back(std::make_shared<Memory>(m_blockSize, m_poolSize));
    Memory::ptr it = m_memoryPool[size];
    for(int i = 0; i < m_poolSize; i++) {
        Coroutine::ptr cor = std::make_shared<Coroutine>(m_blockSize, it->getBlock());
        cor->setIndex(total + i);
        m_cors.push_back(std::make_pair(cor, false));
    }
}

Coroutine::ptr CoroutinePool::getCoroutineInstance() {
    MutexType::Lock lock(m_mutex);

    if(SHERO_UNLICKLY(m_useCount >= m_cors.size())) {
        addCortineInstance();
    }

    for(auto &it : m_cors) {
        if(it.first && !it.second) {
            m_useCount++;
            it.second = true;
            return it.first;
        }
    }

    LOG_ERROR << "getCoroutineInstance() error";
    return nullptr;
}

void CoroutinePool::releaseCoroutine(Coroutine::ptr cor) {
    MutexType::Lock lock(m_mutex);
    int32_t index = cor->getIndex();

    if(index < 0 || index > (int32_t)m_cors.size()) {
        LOG_ERROR << "this coroutine index is invalid";
        return ;
    }

    if(!m_cors[index].second) {
        LOG_WARN << "m_cors[" << index << "] already release";
        return ;
    }

    m_useCount--;
    m_cors[index].second = false;
    return ;
}

}   // namespace shero