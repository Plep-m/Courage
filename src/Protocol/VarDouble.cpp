#include "Courage/Protocol/VarDouble.hpp"
#include <vector>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h> 
#include <stdexcept>

namespace Courage::Protocol
{

	void writeDouble(std::vector<uint8_t> &data, double value)
	{
		uint64_t temp;
		std::memcpy(&temp, &value, sizeof(value));
		temp = htobe64(temp);

		for (int i = 0; i < 8; i++)
			data.push_back(static_cast<uint8_t>((temp >> (56 - i * 8)) & 0xFF));
	}

	double readDouble(const std::vector<uint8_t> &data, size_t &pos)
	{
		if (pos + 8 > data.size())
			throw std::runtime_error("Not enough data to read double");

		uint64_t temp = 0;
		for (int i = 0; i < 8; i++)
			temp = (temp << 8) | data[pos++];

		temp = be64toh(temp);

		double result;
		std::memcpy(&result, &temp, sizeof(result));
		return result;
	}
}