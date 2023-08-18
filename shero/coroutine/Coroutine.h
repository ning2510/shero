#ifndef __SHERO_COROUTINE_H
#define __SHERO_COROUTINE_H

#include "shero/coroutine/Coctx.h"

#include <memory>
#include <stdint.h>
#include <functional>

namespace shero {

class Coroutine {
public:
    typedef std::shared_ptr<Coroutine> ptr;
    Coroutine(int32_t stackSize, char *stackPtr, 
        std::function<void()> cb = nullptr);
    ~Coroutine();

    bool setCallback(const std::function<void()> &cb);

    static void Yield();
    static void Resume(Coroutine *cor);

    static Coroutine *GetMainCoroutine();
    static bool IsMainCoroutine();
    static Coroutine *GetCurCoroutine();

    int32_t getCorId() const { return m_corid; }
    int32_t getStackSize() const { return m_stackSize; }
    char *getStackSp() const { return m_stackSp; }

    int32_t getIndex() const { return m_index; }
    void setIndex(int32_t v) { m_index = v; }

    bool canResume() const { return m_resume; }
    bool isCallingFunc() const { return m_callingFunc; }
    void setCallingFunc(bool v) { m_callingFunc = v; }

public:
    std::function<void()> m_cb;

private:
    Coroutine();

private:
    coctx m_ctx;
    int32_t m_corid;
    int32_t m_stackSize;
    int32_t m_index;

    char *m_stackSp;
    bool m_resume;
    bool m_callingFunc;
};

}   // namespace shero

#endif
