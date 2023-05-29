//
// Created by Bohan Zhang on 2023/5/29.
//

#include <cassert>
#include <unistd.h>
#include <arpa/inet.h>
#include "httpconn.h"

std::atomic<int> HttpConn::user_count;

HttpConn::HttpConn() {
    fd_ = -1;
    addr_ = {0};
    is_close_ = true;
}

HttpConn::~HttpConn() {
    Close();
}

void HttpConn::Init(int sock_fd, const sockaddr_in &addr) {
    assert(sock_fd > 0);
    user_count++;
    addr_ = addr;
    fd_ = sock_fd;
    is_close_ = false;
}

void HttpConn::Close() {
    if (is_close_) {
        return;
    }
    is_close_ = true;
    user_count--;
    close(fd_);
}

int HttpConn::GetFd() const {
    return fd_;
}

int HttpConn::GetPort() const {
    return addr_.sin_port;
}

const char *HttpConn::GetIP() const {
    return inet_ntoa(addr_.sin_addr);
}

sockaddr_in HttpConn::GetAddr() const {
    return addr_;
}


