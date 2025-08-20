// include/Courage/Core/TickTask.hpp
#pragma once
#include <functional>
#include <cstdint>

namespace Courage::Core {

struct TickTask {
    std::function<void(uint64_t)> func;
    bool oneShot{false};
};

}
