cmake_minimum_required(VERSION 3.18)
project(lime_tinyserver)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(server test/main.cpp server/webserver.cpp server/webserver.h server/epoller.h server/epoller.cpp
        pool/threadpool.h http/httpconn.h http/httpconn.cpp)
add_executable(client client/client.cpp)