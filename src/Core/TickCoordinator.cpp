#include "Courage/Core/TickCoordinator.hpp"
#include <thread>

using namespace Courage::Core;
using namespace std::chrono;

TickCoordinator::TickCoordinator(int tps, WorkerPool* pool)
: tps_(tps), tickMs_( milliseconds( tps > 0 ? (1000 / tps) : 50 ) ), pool_(pool) {}

TickCoordinator::~TickCoordinator() { stop(); join(); }

void TickCoordinator::setLogic(LogicFn logic)  { logic_  = std::move(logic); }
void TickCoordinator::setCommit(CommitFn commit){ commit_ = std::move(commit); }

void TickCoordinator::start() {
    if (running_) return;
    running_ = true;
    th_ = std::thread([this]{ loop(); });
}

void TickCoordinator::stop()  { running_ = false; }
void TickCoordinator::join()  { if (th_.joinable()) th_.join(); }

void TickCoordinator::loop() {
    TickContext ctx;
    ctx.tickDuration = tickMs_;
    while (running_) {
        auto start = steady_clock::now();
        ctx.start = start;
        ctx.tickIndex = ++tickIndex_;

        if (logic_) logic_(ctx);

        if (commit_) commit_(ctx);

        auto elapsed = steady_clock::now() - start;
        auto remaining = tickMs_ - duration_cast<milliseconds>(elapsed);
        if (remaining.count() > 0) std::this_thread::sleep_for(remaining);
    }
}
