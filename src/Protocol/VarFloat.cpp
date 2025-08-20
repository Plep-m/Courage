#include "Courage/Protocol/VarFloat.hpp"
#include <vector>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>

namespace Courage::Protocol
{
	void writeFloat(std::vector<uint8_t> &data, float value)
	{
		uint32_t temp;
		std::memcpy(&temp, &value, sizeof(value));
		temp = htonl(temp);

		for (int i = 0; i < 4; i++)
			data.push_back(static_cast<uint8_t>((temp >> (24 - i * 8)) & 0xFF));
	}

	float readFloat(const std::vector<uint8_t> &data, size_t &pos)
	{
		if (pos + 4 > data.size())
			throw std::runtime_error("Not enough data to read float");

		uint32_t temp = 0;
		for (int i = 0; i < 4; i++)
			temp = (temp << 8) | data[pos++];

		temp = ntohl(temp);

		float result;
		std::memcpy(&result, &temp, sizeof(result));
		return result;
	}
}