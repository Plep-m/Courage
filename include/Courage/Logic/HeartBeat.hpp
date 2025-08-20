#pragma once
#include "Courage/Core/TickRegistry.hpp"
#include <cstdint>

namespace Courage::Logic {

class HeartBeat {
public:
    using Registry = Courage::Core::TickRegistry;

    explicit HeartBeat(Registry& registry);

private:
    void registerTasks(Registry& registry);
};

}
