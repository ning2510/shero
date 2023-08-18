#include "shero/base/Log.h"
#include "shero/coroutine/CoroutinePool.h"

#include <iostream>

void func1() {
    LOG_INFO << "[11111] start";
    shero::Coroutine::Yield();
    LOG_INFO << "[11111] end";
    return ;
}

void func2() {
    LOG_INFO << "[22222] start";
    shero::Coroutine::Yield();
    LOG_INFO << "[22222] end";
    return ;
}

int main() {
    shero::Coroutine::ptr cor1 =
        shero::CoroutinePool::GetCoroutinePool()->getCoroutineInstance();
    cor1->setCallback(func1);

    shero::Coroutine::ptr cor2 =
        shero::CoroutinePool::GetCoroutinePool()->getCoroutineInstance();
    cor2->setCallback(func2);

    std::cout << shero::Coroutine::GetMainCoroutine() << ' '
        << cor1.get() << ' ' << cor2.get() << std::endl;

    LOG_INFO << "[00000] main start";
    shero::Coroutine::Resume(cor1.get());
    shero::Coroutine::Resume(cor2.get());
    
    shero::Coroutine::Resume(cor2.get());
    shero::Coroutine::Resume(cor1.get());
    LOG_INFO << "[00000] main end";

    return 0;
}