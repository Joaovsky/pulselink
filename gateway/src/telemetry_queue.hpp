#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T>
class TelemetryQueue {
public:
    void push(T value) {
        { std::lock_guard<std::mutex> lock(mtx_); queue_.push(std::move(value)); }
        cv_.notify_one();
    }
    std::optional<T> wait_pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return closed_ || !queue_.empty(); });
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
    void close() {
        { std::lock_guard<std::mutex> lock(mtx_); closed_ = true; }
        cv_.notify_all();
    }
private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool closed_ = false;
};
