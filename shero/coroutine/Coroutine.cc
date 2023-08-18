#include "shero/base/Log.h"
#include "shero/base/Macro.h"
#include "shero/coroutine/Coroutine.h"

#include <atomic>
#include <assert.h>
#include <string.h>
#include <iostream>

namespace shero {

static thread_local Coroutine::ptr t_mainCoroutine_ptr = nullptr;
static thread_local Coroutine *t_mainCoroutine = nullptr;
static thread_local Coroutine *t_curCoroutine = nullptr;

static thread_local std::atomic_int32_t t_curCoroutineid = {1};

void swapCorFunc(Coroutine *cor) {
    if(cor) {
        cor->setCallingFunc(true);
        cor->m_cb();
        cor->setCallingFunc(false);
    }
    Coroutine::Yield();
}

// static
Coroutine *Coroutine::GetMainCoroutine() {
    if(SHERO_UNLICKLY(!t_mainCoroutine_ptr)) {
        t_mainCoroutine_ptr.reset(new Coroutine());
        t_mainCoroutine = t_mainCoroutine_ptr.get();
    }
    return t_mainCoroutine;
}

Coroutine *Coroutine::GetCurCoroutine() {
    if(SHERO_UNLICKLY(!t_curCoroutine)) {
        GetMainCoroutine();
        t_curCoroutine = t_mainCoroutine;
    }
    return t_curCoroutine;
}

bool Coroutine::IsMainCoroutine() {
    return t_curCoroutine == t_mainCoroutine;
}

Coroutine::Coroutine()
    : m_corid(0),
      m_stackSize(0),
      m_stackSp(nullptr),
      m_resume(true),
      m_callingFunc(false) {
    m_corid = 0;
    memset(&m_ctx, 0, sizeof(m_ctx));
    t_curCoroutine = this;
}

Coroutine::Coroutine(int32_t stackSize, char *stackPtr, 
        std::function<void()> cb /*= nullptr*/)
    : m_corid(0),
      m_stackSize(stackSize),
      m_stackSp(stackPtr),
      m_resume(true),
      m_callingFunc(false) {
    
    assert(stackPtr);
    
    GetMainCoroutine();

    if(cb) {
        setCallback(cb);
    }

    m_corid = t_curCoroutineid++;
}

Coroutine::~Coroutine() {
    // if(this == t_mainCoroutine) {
    //     std::cout << "~Coroutine\n";
    // }
}

bool Coroutine::setCallback(const std::function<void()> &cb) {
    if(this == t_mainCoroutine) {
        LOG_ERROR << "main coroutine can't set callback";
        return false;
    }

    if(m_callingFunc) {
        LOG_ERROR << "cur coroutine is calling function";
        return false;
    }

    m_cb = std::move(cb);
    m_resume = true;

    char *sp = m_stackSp + m_stackSize;
    sp = reinterpret_cast<char*>((reinterpret_cast<unsigned long>(sp)) & -16LL);

    memset(&m_ctx, 0, sizeof(m_ctx));
    m_ctx.regs[kRSP] = sp;
    m_ctx.regs[kRBP] = sp;
    m_ctx.regs[kRETAddr] = reinterpret_cast<char *>(swapCorFunc); 
    m_ctx.regs[kRDI] = reinterpret_cast<char *>(this);

    return true;
}

void Coroutine::Yield() {
    if(!t_mainCoroutine || t_mainCoroutine == t_curCoroutine) {
        LOG_ERROR << "g_mainCoroutine is nullptr or g_mainCoroutine == g_curCoroutine";
        return;
    }

    Coroutine *cur = t_curCoroutine;
    t_curCoroutine = t_mainCoroutine;
    coctx_swap(&(cur->m_ctx), &(t_mainCoroutine->m_ctx));
}

void Coroutine::Resume(Coroutine *cor) {
    if(t_curCoroutine != t_mainCoroutine) {
        LOG_ERROR << "current coroutine isn't main coroutine";
        return ;
    }

    if(t_mainCoroutine == nullptr) {
        LOG_ERROR << "main coroutine is nullptr";
        return ;
    }

    if(!cor || !cor->m_resume) {
        LOG_ERROR << "coroutine is nullptr or coroutine isn't resume";
        return ;
    }

    if(t_curCoroutine == cor) {
        LOG_ERROR << "current coroutine is equal to resume coroutine";
        return ;
    }

    t_curCoroutine = cor;
    coctx_swap(&(t_mainCoroutine->m_ctx), &(cor->m_ctx));
}

}   // namespace shero