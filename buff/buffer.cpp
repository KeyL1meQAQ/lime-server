//
// Created by Bohan Zhang on 2023/5/29.
//

#include <cassert>
#include <cerrno>
#include "buffer.h"
#include <sys/uio.h>
#include <string>
#include <iostream>

Buffer::Buffer(int init_size) : buffer_(init_size), read_pos_(0), write_pos_(0) {}

size_t Buffer::WritableBytes() const {
    return buffer_.size() - write_pos_;
}

size_t Buffer::ReadableBytes() const {
    return write_pos_ - read_pos_;
}

// 获取已经被读取的字节数
size_t Buffer::PrependableBytes() const {
    return read_pos_;
}

char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

char* Buffer::BeginWrite() {
    return BeginPtr_() + write_pos_;
}

// 更新write_pos_
void Buffer::HasWritten(size_t len) {
    write_pos_ += len;
}

// 确保有足够的空间
void Buffer::EnsureWriteable(size_t len) {
    if (WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

// 重新分配空间
void Buffer::MakeSpace_(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(write_pos_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + read_pos_, BeginPtr_() + write_pos_, BeginPtr_());
        read_pos_ = 0;
        write_pos_ = read_pos_ + readable;
        assert(readable == ReadableBytes());
    }
}

// 将数据添加到buffer中
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    // 每次添加数据后，更新write_pos_
    HasWritten(len);
}

// 从fd中读取数据
ssize_t Buffer::ReadFd_(int fd, int *error) {
    char buffer[65535];

    struct iovec iov[2];
    const size_t writable = WritableBytes();

    iov[0].iov_base = BeginPtr_() + write_pos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buffer;
    iov[1].iov_len = sizeof(buffer);

    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *error = errno;
    } else if (static_cast<size_t>(len) <= writable) {
        write_pos_ += len;
    } else {
        write_pos_ = buffer_.size();
        Append(buffer, len - writable);
    }
    if (len > 0) {
        std::string data;
        for (auto& c : buffer_) {
            data += c;
        }
        std::cout << "receive data: " << data << std::endl;
    }
    return len;
}