#include "Courage/Protocol/VarInt.hpp"
#include <stdexcept>
#include <sys/socket.h>

namespace Courage::Protocol
{

	int32_t readVarInt(int sock)
	{
		int numRead = 0;
		int32_t result = 0;
		unsigned char byte = 0;

		do
		{
			if (recv(sock, &byte, 1, 0) <= 0)
				throw std::runtime_error("Connection closed while reading VarInt");

			int value = (byte & 0b01111111);
			result |= (value << (7 * numRead));

			numRead++;
			if (numRead > 5)
				throw std::runtime_error("VarInt too big !");
		} while ((byte & 0b10000000) != 0);

		return result;
	}

	void writeVarInt(std::vector<uint8_t> &buffer, int32_t value)
	{
		do
		{
			unsigned char temp = (unsigned char)(value & 0b01111111);
			value >>= 7;
			if (value != 0)
				temp |= 0b10000000;
			buffer.push_back(temp);
		} while (value != 0);
	}
}
