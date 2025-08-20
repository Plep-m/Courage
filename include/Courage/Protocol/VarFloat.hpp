#pragma once
#include <cstdint>
#include <vector>

namespace Courage::Protocol
{
	void writeFloat(std::vector<uint8_t>& data, float value);
}