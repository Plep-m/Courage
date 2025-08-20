#include "Courage/Network/Server.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/Status.hpp"
#include "Courage/Core/TickRegistry.hpp"

#include "Courage/Logic/HeartBeat.hpp"

#include <csignal>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <stdexcept>

using Courage::Core::WorkerPool;
using Courage::Core::TickCoordinator;
using Courage::Core::ThreadSafeQueue;
using Courage::Core::TickRegistry;
using namespace Courage::Protocol;

namespace Courage::Network {

std::atomic<Server*> Server::instance_{nullptr};

Server::~Server() {
    try { stop(); } catch (...) {}

    if (server_fd >= 0) {
        ::close(server_fd);
        server_fd = -1;
    }
}

Server::Server(int port,
               const Properties& props,
               std::shared_ptr<ThreadSafeQueue<NetEvent>> evtQueue)
    : port(port), props(props), evtQueue_(std::move(evtQueue))
{
    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("Impossible de créer le socket");
    }

    int opt = 1;
    if (::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        ::close(server_fd);
        server_fd = -1;
        throw std::runtime_error("Erreur setsockopt()");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(server_fd);
        server_fd = -1;
        throw std::runtime_error("Erreur bind()");
    }

    if (::listen(server_fd, 5) < 0) {
        ::close(server_fd);
        server_fd = -1;
        throw std::runtime_error("Erreur listen()");
    }
}

void Server::handle_sigint_static(int) {
    if (auto* self = instance_.load()) {
        self->handle_sigint();
    }
}

void Server::handle_sigint() {
    running_.store(false);
    if (server_fd >= 0) {
        ::shutdown(server_fd, SHUT_RDWR);
        ::close(server_fd);
        server_fd = -1;
    }
}

void Server::run() {
    instance_.store(this);

    std::signal(SIGINT, &Server::handle_sigint_static);

    pool_ = std::make_unique<WorkerPool>(std::thread::hardware_concurrency());
    tick_ = std::make_unique<TickCoordinator>(20, pool_.get()); // 20 TPS

    configureDefaultTickLogic();

    netThread_ = std::thread([this]{ netThreadEntry(); });

    tick_->start();

    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (tick_) { tick_->stop(); tick_->join(); }
    if (pool_) { pool_->shutdown(); }
    if (evtQueue_) { evtQueue_->shutdown(); }

    if (netThread_.joinable()) netThread_.join();

    instance_.store(nullptr);
}

void Server::stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        if (server_fd >= 0) {
            ::shutdown(server_fd, SHUT_RDWR);
            ::close(server_fd);
            server_fd = -1;
        }
        if (tick_) { tick_->stop(); }
        if (evtQueue_) { evtQueue_->shutdown(); }
    }
}

void Server::netThreadEntry() {
    try {
        this->start();
    } catch (const std::exception& e) {
        std::cerr << "[ERREUR] Exception dans le thread réseau: " << e.what() << std::endl;
    }
}

void Server::configureDefaultTickLogic() {
    tickRegistry_ = std::make_unique<TickRegistry>();

    Courage::Logic::HeartBeat hb(*tickRegistry_);

    tick_->setLogic([this](auto& ctx){
        if (evtQueue_) {
            for (int i = 0; i < 1000; ++i) {
                auto ev = evtQueue_->try_pop();
                if (!ev) break;
                // ... handle NetEvents ...
            }
        }

        if (tickRegistry_) {
            tickRegistry_->run(ctx.tickIndex);
        }
    });
}

void Server::start() {
    std::cout << "[INFO] Serveur démarré sur le port " << port << std::endl;

    while (running_.load()) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = ::accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (!running_.load()) break;
            continue;
        }

        if (evtQueue_) {
            evtQueue_->push({ EventType::ClientConnected, client_fd, {} });
        }

        try {
            auto handshake = receivePacket(client_fd, -1);
            if (handshake.empty() || handshake[0] != 0x00)
                throw std::runtime_error("Invalid handshake packet");

            int next_state = handshake.back();
            if (next_state == 1)
                handleStatusRequest(client_fd, props);
            else if (next_state == 2)
                handleLoginRequest(client_fd);
        }
        catch (std::exception& e) {
            std::cerr << "[ERREUR] " << e.what() << std::endl;
        }

        if (evtQueue_) {
            evtQueue_->push({ EventType::ClientDisconnected, client_fd, {} });
        }

        ::close(client_fd);
    }

    std::cout << "[INFO] Serveur arrêté (boucle accept terminée)." << std::endl;
}

}
