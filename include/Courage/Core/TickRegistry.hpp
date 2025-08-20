#pragma once
#include "Courage/Core/TickTask.hpp"
#include <mutex>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace Courage::Core {

class TickRegistry {
public:
    void addTask(const TickTask& task) {
        std::scoped_lock lock(mutex_);
        tasks_.push_back(task);
    }

    void addPermanent(std::function<void(uint64_t)> func) {
        addTask({std::move(func), false});
    }

    void addOneShot(std::function<void(uint64_t)> func) {
        addTask({std::move(func), true});
    }

    void run(uint64_t tickIndex) {
        std::scoped_lock lock(mutex_);
        auto it = tasks_.begin();
        while (it != tasks_.end()) {
            it->func(tickIndex);
            if (it->oneShot) {
                it = tasks_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::mutex mutex_;
    std::vector<TickTask> tasks_;
};

}
