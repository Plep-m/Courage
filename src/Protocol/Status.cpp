#include "Courage/Protocol/Status.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Config/Properties.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

using namespace Courage::Network;
using namespace Courage::Protocol;

namespace Courage::Protocol
{
	void handleStatusRequest(int sock, const Properties& props)
	{
		auto ping = receivePacket(sock, -1);
		if (ping.empty() || ping[0] != Network::PacketIds::STATUS)
			throw std::runtime_error("Invalid ping request");

		std::vector<uint8_t> pong;
		writeVarInt(pong, Network::PacketIds::STATUS);
		pong.insert(pong.end(), ping.begin() + 1, ping.end());
		sendPacket(sock, pong, -1);
	}
}
