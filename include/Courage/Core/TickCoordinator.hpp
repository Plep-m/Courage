#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>
#include "Courage/Core/TickTypes.hpp"
#include "Courage/Core/WorkerPool.hpp"

namespace Courage::Core {

class TickCoordinator {
public:
    using LogicFn = std::function<void(TickContext&)>;
    using CommitFn = std::function<void(TickContext&)>;

    TickCoordinator(int tps, WorkerPool* pool);
    ~TickCoordinator();

    void setLogic(LogicFn logic);
    void setCommit(CommitFn commit);

    void start();
    void stop();
    void join();

private:
    void loop();

    int tps_ = 20;
    std::chrono::milliseconds tickMs_{50};
    WorkerPool* pool_ = nullptr;

    std::thread th_;
    std::atomic<bool> running_{false};
    LogicFn logic_;
    CommitFn commit_;
    std::uint64_t tickIndex_{0};
};

}
