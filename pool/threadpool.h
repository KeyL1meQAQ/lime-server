//
// Created by Bohan Zhang on 2023/5/29.
//

#ifndef LIME_TINYSERVER_THREADPOOL_H
#define LIME_TINYSERVER_THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

class ThreadPool {
public:
    explicit ThreadPool(int thread_num = 8): pool_(std::make_shared<Pool>()) {
        for (size_t i = 0; i < thread_num; ++i) {
            std::thread( [pool = pool_] {
                std::unique_lock<std::mutex> locker(pool->mtx);
                while (true) {
                    if (!pool->tasks.empty()) { // 有任务
                        // 取出一个任务
                        auto task = std::move(pool->tasks.front());
                        // 删除该任务
                        pool->tasks.pop();
                        // 任务执行需要一定时间，所以先解锁
                        locker.unlock();
                        task();
                        // 任务执行完毕，重新加锁
                        locker.lock();
                    } else if (pool->is_closed) {
                        break;
                    } else {
                        pool->cv.wait(locker); // 等待新的任务
                    }
                }
            }).detach();
        }
    }
    ThreadPool() = default;
    ThreadPool(ThreadPool&) = default;

    ~ThreadPool() {
        if (static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> guard(pool_->mtx);
                pool_->is_closed = true;
            }
            pool_->cv.notify_all();
        }
    }

    template<typename T>
    void AddTask(T&& task) {
        if (static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> guard(pool_->mtx);
                pool_->tasks.emplace(std::forward<T>(task));
            }
            pool_->cv.notify_one();
        }
    }

    struct Pool {
        std::mutex mtx;
        std::condition_variable cv;
        bool is_closed = false;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

#endif //LIME_TINYSERVER_THREADPOOL_H
