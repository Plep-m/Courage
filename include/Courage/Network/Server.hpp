#pragma once
#include "Courage/Config/Properties.hpp"
#include "Courage/Core/WorkerPool.hpp"
#include "Courage/Core/TickCoordinator.hpp"
#include "Courage/Core/ThreadSafeQueue.hpp"
#include "Courage/Core/TickRegistry.hpp"
#include "Courage/Network/NetEvent.hpp"
#include <atomic>
#include <memory>
#include <thread>

namespace Courage::Network {

class ConnectionManager;
class EventDispatcher;

class Server {
public:
    ~Server();
    Server(int port,
           const Properties& props,
           std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue);

    void run();
    void stop();

private:
    void handle_sigint();
    static void handle_sigint_static(int);
    void netThreadEntry();

    int port_;
    std::atomic<bool> running_{true};
    const Properties* props_{nullptr};

    // Ownership
    std::unique_ptr<Courage::Core::WorkerPool> pool_;
    std::unique_ptr<Courage::Core::TickCoordinator> tick_;
    std::unique_ptr<Courage::Core::TickRegistry> tickRegistry_;

    std::unique_ptr<ConnectionManager> connections_;
    std::unique_ptr<EventDispatcher> dispatcher_;

    std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue_;
    std::thread netThread_;

    static std::atomic<Server*> instance_;
};

}
