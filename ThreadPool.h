#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <exception>
#include <mutex>
#include <condition_variable>

class ThreadPool;
inline ThreadPool* g_thread_pool = nullptr;

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t num_threads) {
        for (std::size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] {
                for (;;) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    try {
                        task();
                    }
                    catch (const std::exception& ex) {
                        // Handle exception
                        std::cout << ex.what() << "\n";
                        throw std::exception(ex);
                    }
                }
                });
        }
        g_thread_pool = this;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
        g_thread_pool = nullptr;
    }

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.emplace([task]() { std::invoke(*task); });
        }
        cv_.notify_one();
        return res;
    }

    template <typename F, typename... Args>
    auto check_return_value(F&& f, Args&&... args)
        -> std::invoke_result_t<F, Args...> {
        auto result = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
            std::cout << "The function did not return a value" << std::endl;
        }
        else {
            std::cout << "The function returned a value" << std::endl;
        }
        return result;
    }

private:
    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};
