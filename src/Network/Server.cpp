#include "Courage/Network/Server.hpp"
#include "Courage/Network/ConnectionManager.hpp"
#include "Courage/Network/EventDispatcher.hpp"
#include <csignal>
#include <chrono>
#include <iostream>

using namespace Courage::Core;
using namespace Courage::Network;

std::atomic<Server*> Server::instance_{nullptr};

Server::Server(int port,
               const Properties& props,
               std::shared_ptr<ThreadSafeQueue<NetEvent>> evtQueue)
: port_(port),
  props_(&props),
  evtQueue_(std::move(evtQueue)) 
{
    pool_ = std::make_unique<WorkerPool>(std::thread::hardware_concurrency());
    connections_ = std::make_unique<ConnectionManager>(port_, evtQueue_, pool_.get());

    dispatcher_ = std::make_unique<EventDispatcher>(evtQueue_, *props_, connections_.get());
    dispatcher_->attachPool(pool_.get());

    tick_ = std::make_unique<TickCoordinator>(20, pool_.get());
    tickRegistry_ = std::make_unique<TickRegistry>();
}

Server::~Server() {
    try { stop(); } catch (...) {}
}

void Server::handle_sigint_static(int) {
    if (auto* self = instance_.load()) self->handle_sigint();
}

void Server::handle_sigint() {
    running_.store(false);
    if (connections_) connections_->shutdownListening();
}

void Server::run() {
    instance_.store(this);
    std::signal(SIGINT, &Server::handle_sigint_static);

    tick_->setLogic([this](auto& ctx){
        dispatcher_->drainAndDispatch(1000);
        if (tickRegistry_) tickRegistry_->run(ctx.tickIndex);
    });

    netThread_ = std::thread([this]{ netThreadEntry(); });

    tick_->start();

    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    tick_->stop();
    tick_->join();

    if (pool_) pool_->shutdown();
    if (evtQueue_) evtQueue_->shutdown();
    if (netThread_.joinable()) netThread_.join();

    instance_.store(nullptr);
}

void Server::stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        if (connections_) connections_->shutdownListening();
        if (tick_) tick_->stop();
        if (evtQueue_) evtQueue_->shutdown();
    }
}

void Server::netThreadEntry() {
    try {
        connections_->startListening(running_);
    } catch (const std::exception& e) {
        std::cerr << "[ERREUR] Exception dans le thread réseau: " << e.what() << std::endl;
        running_.store(false);
    }
}
