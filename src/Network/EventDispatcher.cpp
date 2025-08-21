#include "Courage/Network/EventDispatcher.hpp"
#include "Courage/Network/ConnectionManager.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/Status.hpp"
#include "Courage/Protocol/Login.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include <iostream>

using namespace Courage::Core;
using namespace Courage::Network;
using namespace Courage::Protocol;

static std::string readString(const std::vector<uint8_t>& data, size_t& offset) {
    int32_t len = Courage::Protocol::readVarInt(data, offset);
    if (offset + len > data.size())
        throw std::runtime_error("String length out of bounds");
    std::string result(data.begin() + offset, data.begin() + offset + len);
    offset += len;
    return result;
}

static uint16_t readUnsignedShort(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset + 2 > data.size())
        throw std::runtime_error("UnsignedShort out of bounds");
    uint16_t value = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    return value;
}

EventDispatcher::EventDispatcher(
    std::shared_ptr<ThreadSafeQueue<NetEvent>> queue,
    const Properties& props,
    ConnectionManager* connMgr)
: evtQueue_(std::move(queue)), props_(props), connMgr_(connMgr) {}

void EventDispatcher::attachPool(WorkerPool* pool) {
    pool_ = pool;
}

void EventDispatcher::drainAndDispatch(int maxEvents) {
    if (!evtQueue_) return;
    for (int i = 0; i < maxEvents; ++i) {
        auto ev = evtQueue_->try_pop();
        if (!ev) break;
        dispatch(*ev);
    }
}

void EventDispatcher::dispatch(const NetEvent& ev) {
    switch (ev.type) {
        case EventType::ClientConnected:
            std::cout << "[INFO] Client connected fd=" << ev.clientFd << "\n";
            break;

        case EventType::PacketReceived:
            if (!ev.payload) break;
            if (pool_) {
                pool_->submit([this, ev] {
                    handlePacket(ev.clientFd, *ev.payload);
                });
            }
            break;

        case EventType::ClientDisconnected:
            std::cout << "[INFO] Client disconnected fd=" << ev.clientFd << "\n";
            break;

        default:
            break;
    }
}

void EventDispatcher::handlePacket(int clientFd, const std::vector<unsigned char>& data) {
    size_t offset = 0;
    int32_t packetId = Courage::Protocol::readVarInt(data, offset);

    auto state = connMgr_->getState(clientFd);

    if (state == Connection::ConnState::Handshaking) {
        if (packetId != 0x00) {
            std::cerr << "[WARN] Expected Handshake packet (0x00) in Handshaking state\n";
            return;
        }

        // Parse Handshake
        int protocolVersion = readVarInt(data, offset);
        std::string serverAddress = readString(data, offset);
        uint16_t serverPort = readUnsignedShort(data, offset);
        int nextState = readVarInt(data, offset);

        if (nextState == 1) {
            connMgr_->setState(clientFd, Connection::ConnState::Play);
        } else if (nextState == 2) {
            connMgr_->setState(clientFd, Connection::ConnState::Play);
        }

        std::cout << "[INFO] Handshake received, nextState=" << nextState << "\n";
        return;
    }

    switch (packetId) {
        case 0x00: {
            Courage::Protocol::handleStatusRequest({EventType::PacketReceived, clientFd,
                std::make_shared<std::vector<uint8_t>>(data)}, props_);
            break;
        }

        case 0x01: {
            std::vector<uint8_t> pongPacket;
            writeVarInt(pongPacket, 0x01);
            pongPacket.insert(pongPacket.end(),
                              data.begin() + offset,
                              data.begin() + offset + 8);

            sendPacket(clientFd, pongPacket, -1);
            break;
        }

        case 0x02: {
            Courage::Protocol::handleLoginRequest({EventType::PacketReceived, clientFd,
                std::make_shared<std::vector<uint8_t>>(data)});
            break;
        }

        default:
            std::cerr << "[WARN] Unknown packet id: " << packetId << std::endl;
            break;
    }
}
