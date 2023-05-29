#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(8080);
    // 连接服务器
    int ret = connect(fd, (struct sockaddr*)&seraddr, sizeof(seraddr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }

    char sendBuf[1024] = {0};
    while (1) {
        fgets(sendBuf, sizeof(sendBuf), stdin);
        write(fd, sendBuf, strlen(sendBuf) + 1);
    }

    close(fd);
    return 0;
}