#include "Courage/Config/Properties.hpp"
#include "Courage/Network/Server.hpp"
#include "Courage/Network/NetEvent.hpp"
#include "Courage/Core/ThreadSafeQueue.hpp"

using Courage::Core::ThreadSafeQueue;
using Courage::Network::NetEvent;

int main() {
    Properties props("server.properties");
    int port = props.getInt("server-port", 25565);

    auto evtQueue = std::make_shared<ThreadSafeQueue<NetEvent>>();
    Courage::Network::Server server(port, props, evtQueue);

    // All orchestration (signal handling, tick loop, worker pool, net thread) is inside run()
    server.run();

    return 0;
}
