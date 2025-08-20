#include "Courage/Core/WorkerPool.hpp"

using namespace Courage::Core;

WorkerPool::WorkerPool(std::size_t threadCount) {
    if (threadCount == 0) threadCount = 1;
    threads_.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        threads_.emplace_back([this]{ workerLoop(); });
    }
}

WorkerPool::~WorkerPool() {
    shutdown();
}

void WorkerPool::shutdown() {
    running_ = false;
    cv_.notify_all();
    for (auto& thread : threads_) {
        if (thread.joinable()) thread.join();
    }
}

void WorkerPool::workerLoop() {
    while (running_) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lk(m_);
            cv_.wait(lk, [this]{ return !jobs_.empty() || !running_; });
            if (!running_ && jobs_.empty()) return;
            job = std::move(jobs_.front());
            jobs_.pop();
        }
        job();
    }
}
