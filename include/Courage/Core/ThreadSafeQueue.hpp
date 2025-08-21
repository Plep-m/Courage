#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace Courage::Core {

template<typename T>
class ThreadSafeQueue {
public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lk(m_);
            q_.push(std::move(item));
        }
        cv_.notify_one();
    }

    T pop_wait() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]{ return !q_.empty() || !running_; });
        if (!running_ && q_.empty()) return T{};
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lk(m_);
        if (q_.empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    T& back() {
        std::lock_guard<std::mutex> lk(m_);
        return q_.back();
    }

    const T& back() const {
        std::lock_guard<std::mutex> lk(m_);
        return q_.back();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(m_);
        return q_.empty();
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lk(m_);
            running_ = false;
        }
        cv_.notify_all();
    }

private:
    mutable std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;
    bool running_ = true;
};

}
