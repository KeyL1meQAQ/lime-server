//
// Created by Bohan Zhang on 2023/5/29.
//

#ifndef LIME_TINYSERVER_BUFFER_H
#define LIME_TINYSERVER_BUFFER_H

#include <cstring>
#include <cstdio>
#include <vector>
#include <atomic>

class Buffer {
public:
    Buffer(int init_size = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;

    char* BeginWrite();

    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Append(const char* str, size_t len);

    ssize_t ReadFd_(int fd, int* Error);

private:
    char* BeginPtr_();
    const char* BeginPtr_() const;

    void MakeSpace_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> read_pos_;
    std::atomic<std::size_t> write_pos_;
};

#endif //LIME_TINYSERVER_BUFFER_H
