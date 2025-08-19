#pragma once
#include <cstdint>
#include <vector>

namespace Courage::Protocol
{
	int32_t readVarInt(int sock);
	void writeVarInt(std::vector<uint8_t> &buffer, int32_t value);
}