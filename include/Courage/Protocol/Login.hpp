#pragma once
#include "Courage/Network/NetEvent.hpp"

using Courage::Network::NetEvent;
namespace Courage::Protocol
{
    void handleLoginRequest(const NetEvent& ev);
}
