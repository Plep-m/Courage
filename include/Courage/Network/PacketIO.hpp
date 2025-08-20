#pragma once
#include <string>
#include <vector>
#include <stdint.h>

namespace Courage::Network
{
	void sendPacket(int sock, const std::vector<uint8_t> &data, int compressionThreshold);
	std::vector<uint8_t> receivePacket(int sock, int compressionThreshold);
}
