#pragma once
#include "Courage/Network/NetEvent.hpp"
#include "Courage/Network/ConnectionManager.hpp"
#include "Courage/Config/Properties.hpp"
#include "Courage/Core/WorkerPool.hpp"
#include "Courage/Core/ThreadSafeQueue.hpp"
#include <memory>

namespace Courage::Network {

class EventDispatcher {
public:
    EventDispatcher(std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> queue,
                    const Properties& props, ConnectionManager* connMgr);

    void attachPool(Courage::Core::WorkerPool* pool);

    // Drain up to maxEvents events from queue and dispatch them
    void drainAndDispatch(int maxEvents);

private:
    void dispatch(const NetEvent& ev);
    void handlePacket(int clientFd, const std::vector<uint8_t>& data);

    std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue_;
    const Properties& props_;
    Courage::Core::WorkerPool* pool_{nullptr};
    ConnectionManager* connMgr_{nullptr};
};

}
