#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool;
inline ThreadPool* g_thread_pool;

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t numThreads)
        : stop_(false)
    {
        for (std::size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex_);
                        this->condition_.wait(lock,
                            [this] { return this->stop_ || !this->tasks_.empty(); });
                        if (this->stop_ && this->tasks_.empty()) {
                            return;
                        }
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
                });
        }
        g_thread_pool = this;
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
        g_thread_pool = nullptr;
    }

    template <typename T>
    void enqueue(T task)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            tasks_.emplace(task);
        }
        condition_.notify_one();
    }

private:
    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;

    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
};