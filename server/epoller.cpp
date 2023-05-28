//
// Created by Bohan Zhang on 2023/5/28.
//

#include <cassert>
#include <unistd.h>
#include "epoller.h"

// 构建Epoll文件描述符，并且初始化events_数组大小
Epoller::Epoller(int max_events): epoll_fd_(epoll_create(512)), events_(max_events) {
    assert(epoll_fd_ >= 0 && !events_.empty());
}

Epoller::~Epoller() {
    // 在析构时关闭Epoll文件描述符
    close(epoll_fd_);
}

// 调用epoll_ctl函数，向epoll中添加文件描述符
bool Epoller::AddFd(int fd, uint32_t events) {
    if (fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

// 调用epoll_ctl函数，修改epoll中的文件描述符
bool Epoller::ModFd(int fd, uint32_t events) {
    if (fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

// 调用epoll_ctl函数，删除epoll中的文件描述符
bool Epoller::DelFd(int fd) {
    if (fd < 0) return false;
    epoll_event ev = {0};
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) == 0;
}

// 调用epoll_wait函数，等待事件的发生
int Epoller::Wait(int timeout_ms) {
    return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()), timeout_ms);
}

// 返回事件的文件描述符
int Epoller::GetEventFd(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].data.fd;
}

// 返回事件的类型
uint32_t Epoller::GetEvents(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].events;
}


