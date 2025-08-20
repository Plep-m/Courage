#pragma once
#include <vector>
#include <cstdint>

namespace Courage::Network {

enum class EventType : uint8_t {
    ClientConnected,
    ClientDisconnected,
    PacketReceived
};

struct NetEvent {
    EventType type{};
    int clientFd{-1};
    std::shared_ptr<std::vector<uint8_t>> payload;
};

}
