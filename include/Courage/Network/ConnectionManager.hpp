#pragma once
#include "Courage/Network/NetEvent.hpp"
#include "Courage/Core/ThreadSafeQueue.hpp"
#include "Courage/Core/WorkerPool.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace Courage::Network {

struct Connection {
    int fd{-1};
    enum class ConnState { Handshaking, Play } state{ConnState::Handshaking};
};

class ConnectionManager {
public:
    ConnectionManager(int port,
                      std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue,
                      Courage::Core::WorkerPool* pool);
    ~ConnectionManager();

    void startListening(std::atomic<bool>& running);
    void shutdownListening();

    Connection::ConnState getState(int fd);
    void setState(int fd, Connection::ConnState newState);

private:
    void acceptLoop(std::atomic<bool>& running);
    void closeServerSocket();
    void pushEvent(NetEvent ev);

    int port_;
    int server_fd_{-1};
    std::shared_ptr<Courage::Core::ThreadSafeQueue<NetEvent>> evtQueue_;

    std::mutex connsMutex_;
    std::unordered_map<int, Connection> conns_;

    Courage::Core::WorkerPool* pool_{nullptr};
};

}
