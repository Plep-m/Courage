#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <functional>

#include "Courage/Core/ThreadSafeQueue.hpp"
#include "Courage/Core/WorkerPool.hpp"
#include "Courage/Core/TickCoordinator.hpp"
#include "Courage/Core/TickRegistry.hpp"
#include "Courage/Network/NetEvent.hpp"
#include "Courage/Config/Properties.hpp"

namespace Courage::Network {

class Server {
public:
    Server(int port,
           const Properties& props,
           std::shared_ptr<Courage::Core::ThreadSafeQueue<Courage::Network::NetEvent>> evtQueue = nullptr);
    ~Server();

    void run();
    void stop();
    void start();

private:
    // Signal handler hook
    static void handle_sigint_static(int);
    void handle_sigint();

    void netThreadEntry();

    void configureDefaultTickLogic();

private:
    int port;
    int server_fd{-1};
    const Properties& props;

    std::shared_ptr<Courage::Core::ThreadSafeQueue<Courage::Network::NetEvent>> evtQueue_;

    std::unique_ptr<Courage::Core::WorkerPool> pool_;
    std::unique_ptr<Courage::Core::TickCoordinator> tick_;

	std::unique_ptr<Courage::Core::TickRegistry> tickRegistry_;

    std::thread netThread_;
    std::atomic<bool> running_{true};

    static std::atomic<Server*> instance_;
};

}
