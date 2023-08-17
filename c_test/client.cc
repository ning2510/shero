#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char **argv) {
    uint16_t port = 9999;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        cout << "socket error" << endl;
        return 0;
    }

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("10.13.0.39");

    int rt = connect(fd, (struct sockaddr *)&addr, len);
    if(rt < 0) {
        cout << "connect error, errno = " << errno << " errstr = " << strerror(errno) << endl;
        return 0;
    }

    while(1) {
        string msg;
        cin >> msg;
        rt = send(fd, &msg[0], msg.size(), 0);
        cout << "send rt = " << rt << endl;

        char buff[1024] = {0};
        int rt = recv(fd, buff, sizeof(buff), 0);
        cout << "recv rt = " << rt << endl;
        cout << "msg = " << buff;
    }

    return 0;
}