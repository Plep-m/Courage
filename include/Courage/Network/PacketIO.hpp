#pragma once
#include <string>
#include <vector>

namespace Courage::Network
{
	void sendPacket(int sock, const std::vector<uint8_t> &data);
	std::vector<uint8_t> receivePacket(int sock);
}
