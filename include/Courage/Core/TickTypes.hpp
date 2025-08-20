#pragma once
#include <cstdint>
#include <chrono>

namespace Courage::Core {

struct TickContext {
    std::uint64_t tickIndex = 0;
    std::chrono::steady_clock::time_point start;
    std::chrono::milliseconds tickDuration{50};
};

}
