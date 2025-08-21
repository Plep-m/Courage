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
#include <mutex>
#include <unordered_map>

namespace Courage::Network {

struct Connection {
    int fd;
    enum class ConnState { Handshaking, Play } state;
};

class Server {
public:
    ~Server();
    Server(int port, const Properties& props, std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue);
    void run();
    void stop();
    void dispatchEvent(const NetEvent& ev);

private:
    void start();
    void netThreadEntry();
    void configureDefaultTickLogic();
    void handle_sigint();
    static void handle_sigint_static(int);

    int server_fd{-1};
    int port;
    std::atomic<bool> running_{true};
    const Properties* props_;
    std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue_;
    std::unique_ptr<Courage::Core::WorkerPool> pool_;
    std::unique_ptr<Courage::Core::TickCoordinator> tick_;
    std::unique_ptr<Courage::Core::TickRegistry> tickRegistry_;
    std::thread netThread_;
    std::mutex connsMutex_;
    std::unordered_map<int, Connection> conns_;
    static std::atomic<Server*> instance_;
};

} // namespace Courage::Network
