#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/base/Timer.h"
#include "shero/net/EventLoop.h"

#include <iostream>
#include <signal.h>

shero::EventLoop loop;

void test_timer() {
    LOG_INFO << "1111111111 test timer success";
}

void test_timer2() {
    loop.quit();
}

int main() {
    std::cout << "current time = " << shero::GetCurrentMS() << std::endl;
    shero::Timer::ptr timer = std::make_shared<shero::Timer>(&loop);
    
    timer->addTimer(1000, test_timer, true);
    timer->addTimer(2500, test_timer2, false);

    loop.loop();
    std::cout << "loop end" << std::endl;

    return 0;
}