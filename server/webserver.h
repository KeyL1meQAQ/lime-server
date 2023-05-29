//
// Created by Bohan Zhang on 2023/5/28.
//

#ifndef LIME_TINYSERVER_WEBSERVER_H
#define LIME_TINYSERVER_WEBSERVER_H

#include <memory>
#include <unordered_map>
#include "epoller.h"
#include "../http/httpconn.h"
#include "../pool/threadpool.h"

class WebServer {
public:
    WebServer(int port, int trig_mode, int timeout_ms, int thread_num);
    ~WebServer();
    void Start();

private:
    bool InitSocket_();
    void InitEventMode_(int trig_mode);
    void AddClient(int fd, sockaddr_in addr);
    static int SetFdNonblock(int fd);
    void DealListen_();
    void DealRead_(HttpConn* client);
    void DealWrite_(HttpConn* client);
    void OnRead_(HttpConn* client);
    void OnWrite_(HttpConn* client);
    void OnProcess_();
    void CloseConn_(HttpConn* client);
    int timeout_ms_;
    bool is_close_;
    int port_;
    int listen_fd_;

    uint32_t listen_event_;
    uint32_t conn_event_;

    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unordered_map<int, HttpConn> users_;
};

#endif //LIME_TINYSERVER_WEBSERVER_H
