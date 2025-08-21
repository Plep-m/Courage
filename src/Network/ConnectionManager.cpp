#include "Courage/Network/ConnectionManager.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

using namespace Courage::Core;
using namespace Courage::Network;
using namespace Courage::Protocol;

ConnectionManager::ConnectionManager(
    int port,
    std::shared_ptr<ThreadSafeQueue<NetEvent>> evtQueue,
    WorkerPool* pool)
: port_(port),
  evtQueue_(std::move(evtQueue)),
  pool_(pool) {}

ConnectionManager::~ConnectionManager() {
    shutdownListening();
}

void ConnectionManager::startListening(std::atomic<bool>& running) {
    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) throw std::runtime_error("Impossible de créer le socket");

    int opt = 1;
    if (::setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        closeServerSocket();
        throw std::runtime_error("Erreur setsockopt()");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (::bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        closeServerSocket();
        throw std::runtime_error("Erreur bind()");
    }

    if (::listen(server_fd_, 1024) < 0) {
        closeServerSocket();
        throw std::runtime_error("Erreur listen()");
    }

    std::cout << "[INFO] Serveur démarré sur le port " << port_ << std::endl;
    acceptLoop(running);
    std::cout << "[INFO] Serveur arrêté (boucle accept terminée)." << std::endl;
}

void ConnectionManager::shutdownListening() {
    if (server_fd_ >= 0) {
        ::shutdown(server_fd_, SHUT_RDWR);
        ::close(server_fd_);
        server_fd_ = -1;
    }
}

void ConnectionManager::acceptLoop(std::atomic<bool>& running) {
    while (running.load()) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = ::accept(server_fd_, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (!running.load()) break;
            continue;
        }

        {
            std::lock_guard<std::mutex> lk(connsMutex_);
            conns_.emplace(client_fd, Connection{client_fd, Connection::ConnState::Handshaking});
        }

        pushEvent({ EventType::ClientConnected, client_fd, {} });

        if (pool_) {
            pool_->submit([this, client_fd, &running] {
                try {
                    while (running.load()) {
                        auto pkt = receivePacket(client_fd, -1);
                        if (pkt.empty()) break;

                        pushEvent({
                            EventType::PacketReceived,
                            client_fd,
                            std::make_shared<std::vector<uint8_t>>(std::move(pkt))
                        });
                    }
                } catch (...) {
                    // swallow errors, treat as disconnect
                }

                ::close(client_fd);
                {
                    std::lock_guard<std::mutex> lk(connsMutex_);
                    conns_.erase(client_fd);
                }
                pushEvent({ EventType::ClientDisconnected, client_fd, {} });
            });
        }
    }
}

void ConnectionManager::pushEvent(NetEvent ev) {
    if (evtQueue_) evtQueue_->push(std::move(ev));
}

void ConnectionManager::closeServerSocket() {
    if (server_fd_ >= 0) {
        ::close(server_fd_);
        server_fd_ = -1;
    }
}

Connection::ConnState ConnectionManager::getState(int fd) {
    std::lock_guard<std::mutex> lk(connsMutex_);
    auto it = conns_.find(fd);
    if (it != conns_.end())
        return it->second.state;
    return Connection::ConnState::Handshaking;
}

void ConnectionManager::setState(int fd, Connection::ConnState newState) {
    std::lock_guard<std::mutex> lk(connsMutex_);
    auto it = conns_.find(fd);
    if (it != conns_.end())
        it->second.state = newState;
}
