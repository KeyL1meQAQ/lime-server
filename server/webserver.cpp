//
// Created by Bohan Zhang on 2023/5/28.
//

#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <cassert>
#include "webserver.h"

WebServer::WebServer(int port, int trig_mode, int timeout_ms, int thread_num): port_(port), timeout_ms_(timeout_ms),
epoller_(new Epoller()), is_close_(false), threadpool_(new ThreadPool(thread_num)) {
    InitEventMode_(trig_mode);
    if (!InitSocket_()) {
        is_close_ = true;
    }
}

WebServer::~WebServer() {
    close(listen_fd_);
    is_close_ = true;
}

void WebServer::Start() {
    while (!is_close_) {
        // 等待事件
        int event_num = epoller_->Wait(timeout_ms_);
        // 处理事件
        for (int i = 0; i < event_num; ++i) {
            int fd = epoller_->GetEventFd(i); // 获取事件对应的文件描述符
            uint32_t events = epoller_->GetEvents(i); // 获取事件类型
            printf("fd: %d\n", fd); // 打印文件描述符

            if (fd == listen_fd_) { // 监听到新连接
                DealListen_();
            } else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                // 关闭连接
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            } else if (events & EPOLLIN) { // 监听到读事件
                assert(users_.count(fd) > 0);
                DealRead_(&users_[fd]); // 交给子线程执行
            } else if (events & EPOLLOUT) { // 监听到写事件
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]); // 交给子线程执行
            } else {
                perror("Unexpected event");
            }
        }
    }
}

void WebServer::CloseConn_(HttpConn* client) {
    assert(client);
    printf("Client[%d] quit!\n", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
}

void WebServer::AddClient(int fd, sockaddr_in addr) {
    assert(fd > 0);
    users_[fd].Init(fd, addr);
    SetFdNonblock(fd);
    epoller_->AddFd(fd, conn_event_ | EPOLLIN);
}

bool WebServer::InitSocket_() {
    // 创建Socket
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        perror("socket");
        return false;
    }

    // 设置端口复用
    int opt_val = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    // 绑定IP和端口
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port_);
    // 绑定
    int ret = bind(listen_fd_, (struct sockaddr *) &s_addr, sizeof(s_addr));
    if (ret == -1) {
        perror("bind");
        return false;
    }

    // 监听
    ret = listen(listen_fd_, 128); // 最大监听数128
    if (ret == -1) {
        perror("listen");
        return false;
    }

    // 将lfd注册到epoller中
    ret = epoller_->AddFd(listen_fd_, EPOLLIN);
    if (ret == -1) {
        perror("Register fd to epoller failed");
        return false;
    }

    // 设置lfd为非阻塞
    SetFdNonblock(listen_fd_);
    return true;
}

void WebServer::DealListen_() {
    // 处理新的客户端连接
    struct sockaddr_in c_addr;
    socklen_t c_addr_len = sizeof(c_addr);
    do {
        int cfd = accept(listen_fd_, (struct sockaddr *) &c_addr, &c_addr_len);
        if (cfd <= 0) {
            return;
        }
        // 获取客户端信息
        char c_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &c_addr.sin_addr, c_ip, sizeof(c_ip));
        unsigned short c_port = ntohs(c_addr.sin_port);
        // 打印客户端信息
        printf("client ip: %s, port: %d\n", c_ip, c_port);
        AddClient(cfd, c_addr);
    } while (listen_event_ & EPOLLET);
}

void WebServer::DealRead_(HttpConn* client) {
    assert(client);
    threadpool_->AddTask(std::bind(&WebServer::OnRead_, this, client));
}

void WebServer::DealWrite_(HttpConn* client) {
    assert(client);
    threadpool_->AddTask(std::bind(&WebServer::OnWrite_, this, client));
}

void WebServer::OnRead_(HttpConn* client) {
    assert(client);
    int ret = -1;
    int read_errno = 0;
    ret = client->read(&read_errno);
    if (ret <= 0 && read_errno != EAGAIN) {
        CloseConn_(client);
        return;
    }
}

void WebServer::OnWrite_(HttpConn *client) {  }

void WebServer::InitEventMode_(int trig_mode) {
    listen_event_ = EPOLLRDHUP;
//    conn_event_ = EPOLLONESHOT | EPOLLRDHUP; // 注册连接事件时，设置为EPOLLONESHOT，防止多个线程处理同一个连接
    switch ((trig_mode)) {
        case 0:
            break;
        case 1:
            conn_event_ |= EPOLLET;
            break;
        case 2:
            listen_event_ |= EPOLLET;
            break;
        default:
            listen_event_ |= EPOLLET;
            conn_event_ |= EPOLLET;
            break;
    }
    HttpConn::isET = (conn_event_ & EPOLLET);
}

int WebServer::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}




