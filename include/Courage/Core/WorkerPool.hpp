#pragma once
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Courage::Core {

class WorkerPool {
public:
    explicit WorkerPool(std::size_t threadCount = std::thread::hardware_concurrency());
    ~WorkerPool();

    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using R = std::invoke_result_t<F, Args...>;
        auto task = std::packaged_task<R()>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<R> fut = task.get_future();
        {
            std::lock_guard<std::mutex> lk(m_);
            if (!running_) throw std::runtime_error("WorkerPool stopped");

            auto taskPtr = std::make_shared<std::packaged_task<R()>>(std::move(task));
            jobs_.emplace([taskPtr]() { (*taskPtr)(); });
        }
        cv_.notify_one();
        return fut;
    }

    void shutdown();

private:
    void workerLoop();

    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> jobs_;
    std::mutex m_;
    std::condition_variable cv_;
    std::atomic<bool> running_{true};
};

}
