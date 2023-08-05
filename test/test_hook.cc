#include "shero/coroutine/Hook.h"

int main(int argc, char **argv) {
    shero::set_hook_enable(true);
    
    read(0, nullptr, 0);
    write(0, nullptr, 0);
    socket(AF_INET, SOCK_STREAM, 0);
    accept(0, nullptr, nullptr);
    connect(0, nullptr, 0);
    sleep(0);

    return 0;
}