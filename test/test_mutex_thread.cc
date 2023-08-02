#include "shero/util.h"
#include "shero/mutex.h"
#include "shero/thread.h"

#include <iostream>

shero::Mutex mutex;
int num = 0;

void func1() {
    pid_t tid = shero::GetThreadId();
    for(int i = 1; i <= 50; i++) {
        shero::Mutex::Lock lock(mutex);
        num++;
        std::cout << "Thread id = " << tid << ", num = " << num << std::endl;
    }
}

void func2() {
    pid_t tid = shero::GetThreadId();
    for(int i = 1; i <= 50; i++) {
        shero::Mutex::Lock lock(mutex);
        num++;
        std::cout << "Thread id = " << tid << ", num = " << num << std::endl;
    }
}

void test_mutex() {
    // pthread_t p1, p2;
    // pthread_create(&p1, nullptr, func1, nullptr);
    // pthread_create(&p2, nullptr, func2, nullptr);

    // pthread_join(p1, nullptr);
    // pthread_join(p2, nullptr);

    shero::Thread::ptr p1 = std::make_shared<shero::Thread>(func1);
    shero::Thread::ptr p2 = std::make_shared<shero::Thread>(func2);
    
}

int main(int argc, char **argv) {
    test_mutex();

    return 0;
}