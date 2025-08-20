#pragma once
#include <cstdint>
#include <vector>

namespace Courage::Protocol
{
	int32_t readVarInt(int sock);
	int32_t readVarInt(const std::vector<uint8_t> &data, size_t &pos);
	void writeVarInt(std::vector<uint8_t> &buffer, int32_t value);
}