#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>

namespace Courage::Protocol
{
	std::string readVarString(const std::vector<uint8_t> &data, size_t &offset);
	void writeVarString(std::vector<uint8_t> &out, const std::string &str);
}