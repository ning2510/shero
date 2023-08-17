#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char **argv) {
    uint16_t port = 9999;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        cout << "socket error" << endl;
        return 0;
    }

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        cout << "setsockopt error, errno = " << errno << " errstr = " << strerror(errno) << endl;
        return 0;
    }

    int rt = bind(fd, (struct sockaddr *)&addr, len);
    if(rt < 0) {
        cout << "bind error, errno = " << errno << " errstr = " << strerror(errno) << endl;
        return 0;
    }

    int ret = listen(fd, 60);
    if (ret < 0) {
        perror("listen()");
        exit(0);
    }

    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));
    int connfd = accept(fd, (struct sockaddr *)&cli_addr, &cli_len);
    if(connfd < 0) {
        cout << "accept error, errno = " << errno << " errstr = " << strerror(errno) << endl;
        return 0;
    }
    std::cout << "connfd = " << connfd << std::endl;

    // while(1) {
        char buff[10240] = {0};
        rt = recv(connfd, buff, sizeof(buff), 0);
        cout << "recv rt = " << rt << endl;
        cout << "msg = " << buff;

        string msg;
        cin >> msg;
        rt = send(connfd, &msg[0], msg.size(), 0);
        cout << "send rt = " << rt << endl;
    // }

    struct sockaddr_in peer;
    socklen_t peer_len = sizeof(peer);
    memset(&peer, 0, sizeof(peer));
    rt = getpeername(connfd, (struct sockaddr *)&peer, &peer_len);
    if(rt < 0) {
        cout << "getpeername error" << endl;
    }

    cout << "port = " << ntohs(peer.sin_port) << endl;


    close(connfd);
    close(fd);

    return 0;
}