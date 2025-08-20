#include "Courage/Logic/HeartBeat.hpp"
#include <iostream>

namespace Courage::Logic {

HeartBeat::HeartBeat(Courage::Core::TickRegistry& registry) {
    registerTasks(registry);
}

void HeartBeat::registerTasks(Courage::Core::TickRegistry& registry) {
    // Permanent heartbeat every 20 ticks
    registry.addPermanent([](uint64_t tick){
        if (tick % 20 == 0) {
            std::cout << "[HEARTBEAT] tick=" << tick << std::endl;
        }
    });

    // Example one-shot: runs once at next tick
    registry.addOneShot([](uint64_t tick){
        std::cout << "[HEARTBEAT] One-shot ran at tick=" << tick << std::endl;
    });
}

} // namespace Courage::Logic
