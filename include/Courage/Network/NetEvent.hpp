#pragma once
#include <vector>
#include <cstdint>
#include <memory>

namespace Courage::Network {

enum class EventType : uint8_t {
    ClientConnected,
    ClientDisconnected,
    PacketReceived,
    Handshake,
    StatusRequest,
    StatusPing,
    LoginStart,
    TransitionState,
    QueueSendPacket
};

struct NetEvent {
    EventType type{};
    int clientFd{-1};
    std::shared_ptr<std::vector<uint8_t>> payload;
    int32_t aux{0};
};

}
