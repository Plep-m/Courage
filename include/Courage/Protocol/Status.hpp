#pragma once
#include <string>
#include "Courage/Config/Properties.hpp"
#include "Courage/Network/NetEvent.hpp"

using Courage::Network::NetEvent;
namespace Courage::Protocol
{
    void handleStatusRequest(const NetEvent& ev, const Properties& props);
}