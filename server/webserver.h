//
// Created by Bohan Zhang on 2023/5/28.
//

#ifndef LIME_TINYSERVER_WEBSERVER_H
#define LIME_TINYSERVER_WEBSERVER_H

#include <memory>
#include "epoller.h"

class WebServer {
public:
    WebServer(int port, int trig_mode, int timeout_ms);
    ~WebServer();
    void Start();

private:
    bool InitSocket_();
    void InitEventMode_(int trig_mode);
    static int SetFdNonblock(int fd);
    void DealListen_();
    void DealRead_();
    void DealWrite_();
    void OnProcess_();
    void CloseConn_();
    int timeout_ms_;
    bool is_close_;
    int port_;
    uint32_t listen_event_;
    uint32_t conn_event_;
    int listen_fd_;
    std::unique_ptr<Epoller> epoller_;
};

#endif //LIME_TINYSERVER_WEBSERVER_H
