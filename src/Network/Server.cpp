#include "Courage/Network/Server.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/Status.hpp"
#include "Courage/Protocol/Login.hpp"
#include "Courage/Core/TickRegistry.hpp"
#include "Courage/Logic/HeartBeat.hpp"
#include <csignal>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

using namespace Courage::Core;
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

Server::Server(int port, const Properties& props, std::shared_ptr<ThreadSafeQueue<NetEvent>> evtQueue)
: port(port), props_(&props), evtQueue_(std::move(evtQueue)) {

    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::runtime_error("Impossible de créer le socket");

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
    if (auto* self = instance_.load()) self->handle_sigint();
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
    while (running_.load()) std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (tick_) { tick_->stop(); tick_->join(); }
    if (pool_) pool_->shutdown();
    if (evtQueue_) evtQueue_->shutdown();
    if (netThread_.joinable()) netThread_.join();

    instance_.store(nullptr);
}

void Server::stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        if (server_fd >= 0) { ::shutdown(server_fd, SHUT_RDWR); ::close(server_fd); server_fd = -1; }
        if (tick_) tick_->stop();
        if (evtQueue_) evtQueue_->shutdown();
    }
}

void Server::netThreadEntry() {
    try { start(); }
    catch (const std::exception& e) { std::cerr << "[ERREUR] Exception dans le thread réseau: " << e.what() << std::endl; }
}

void Server::dispatchEvent(const NetEvent& ev) {
    switch (ev.type) {
        case EventType::StatusRequest:
            pool_->submit([this, ev]{ handleStatusRequest(ev, *props_); });
            break;
        case EventType::StatusPing:
            pool_->submit([this, ev]{ sendPacket(ev.clientFd, *ev.payload, -1); });
            break;
        case EventType::LoginStart:
            pool_->submit([this, ev]{ handleLoginRequest(ev); });
            break;
        default: break;
    }
}

void Server::configureDefaultTickLogic() {
    tickRegistry_ = std::make_unique<TickRegistry>();
    // Courage::Logic::HeartBeat hb(*tickRegistry_);
    tick_->setLogic([this](auto& ctx){
        if (evtQueue_) {
            for (int i = 0; i < 1000; ++i) {
                auto ev = evtQueue_->try_pop();
                if (!ev) break;
                dispatchEvent(*ev);
            }
        }
        if (tickRegistry_) tickRegistry_->run(ctx.tickIndex);
    });
}

void Server::start() {
    std::cout << "[INFO] Serveur démarré sur le port " << port << std::endl;
    while (running_.load()) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = ::accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) { if (!running_.load()) break; continue; }

        { std::lock_guard<std::mutex> lk(connsMutex_);
          conns_.emplace(client_fd, Connection{client_fd, Connection::ConnState::Handshaking}); }

        evtQueue_->push({ EventType::ClientConnected, client_fd, {} });

        try {
            auto hs = receivePacket(client_fd, -1);
            if (hs.empty() || hs[0] != 0x00) throw std::runtime_error("Invalid handshake packet");

            evtQueue_->push({ EventType::Handshake, client_fd, std::make_shared<std::vector<uint8_t>>(std::move(hs)) });
            int next_state = (*evtQueue_->back().payload).back();

            if (next_state == 1) {
                auto req = receivePacket(client_fd, -1);
                evtQueue_->push({ EventType::StatusRequest, client_fd, std::make_shared<std::vector<uint8_t>>(std::move(req)) });
                auto ping = receivePacket(client_fd, -1);
                evtQueue_->push({ EventType::StatusPing, client_fd, std::make_shared<std::vector<uint8_t>>(std::move(ping)) });
            } else if (next_state == 2) {
                auto loginStart = receivePacket(client_fd, -1);
                evtQueue_->push({ EventType::LoginStart, client_fd, std::make_shared<std::vector<uint8_t>>(std::move(loginStart)) });
            }
        } catch (std::exception& e) { std::cerr << "[ERREUR] " << e.what() << std::endl; }
    }
    std::cout << "[INFO] Serveur arrêté (boucle accept terminée)." << std::endl;
}

} // namespace Courage::Network
