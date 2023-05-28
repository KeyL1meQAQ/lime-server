//
// Created by Bohan Zhang on 2023/5/28.
//

#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in s_addr;
    inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8080);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *) &s_addr, sizeof(s_addr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }

    // 发送数据
    char send_buf[1024] = {0};
    // 读取用户输入并发送到服务器
    while (1) {
        // 读取用户输入
        fgets(send_buf, sizeof(send_buf), stdin);
        // 发送数据
        write(fd, send_buf, strlen(send_buf));
        // 接收服务器发回的数据
        int len = read(fd, send_buf, sizeof(send_buf));
        if (len == -1) {
            perror("read");
            return -1;
        } else if (len == 0) {
            printf("服务器已关闭连接\n");
            return 0;
        } else {
            printf("recv: %s\n", send_buf);
        }
    }

    // 关闭连接
    close(fd);
    return 0;
}