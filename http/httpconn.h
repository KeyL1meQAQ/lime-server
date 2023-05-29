//
// Created by Bohan Zhang on 2023/5/29.
//

#ifndef LIME_TINYSERVER_HTTPCONN_H
#define LIME_TINYSERVER_HTTPCONN_H

#include <netinet/in.h>
#include <atomic>

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
    static std::atomic<int> user_count;
private:
    int fd_;
    sockaddr_in addr_;

    bool is_close_;
};

#endif //LIME_TINYSERVER_HTTPCONN_H
