//
// Created by Bohan Zhang on 2023/5/29.
//

#ifndef LIME_TINYSERVER_HTTPCONN_H
#define LIME_TINYSERVER_HTTPCONN_H

#include <netinet/in.h>
#include <atomic>
#include "../buff/buffer.h"

class HttpConn {
public:
    HttpConn();
    ~HttpConn();
    void Init(int sock_fd, const sockaddr_in& addr);
    void Close();
    int GetFd() const;
    int GetPort() const;
    const char *GetIP() const;
    sockaddr_in GetAddr() const;

    ssize_t read(int* save_error);
    ssize_t write(int* save_error);

    static std::atomic<int> user_count;
    static bool isET;
private:
    int fd_;
    sockaddr_in addr_;

    Buffer read_buffer_;

    bool is_close_;
};

#endif //LIME_TINYSERVER_HTTPCONN_H
