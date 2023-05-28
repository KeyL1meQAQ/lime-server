//
// Created by Bohan Zhang on 2023/5/28.
//

#ifndef LIME_TINYSERVER_EPOLLER_H
#define LIME_TINYSERVER_EPOLLER_H

#include <sys/epoll.h>
#include <vector>
class Epoller {
public:
    explicit Epoller(int max_events = 1024);
    ~Epoller();
    bool AddFd(int fd, uint32_t events);
    bool ModFd(int fd, uint32_t events);
    bool DelFd(int fd);
    int Wait(int timeout_ms = -1);
    int GetEventFd(size_t i) const;
    uint32_t GetEvents(size_t i) const;

private:
    int epoll_fd_; // epoll 文件描述符
    std::vector<epoll_event> events_; // epoll_wait 返回的事件的集合
};

#endif //LIME_TINYSERVER_EPOLLER_H
