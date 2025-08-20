#pragma once
#include <cstdint>
#include <vector>

namespace Courage::Protocol
{
	void writeDouble(std::vector<uint8_t> &data, double value);
}