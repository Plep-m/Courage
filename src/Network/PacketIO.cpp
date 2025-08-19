#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <cstring>

using namespace Courage::Protocol;

namespace Courage::Network
{

	void sendPacket(int sock, const std::vector<uint8_t> &data)
	{
		std::vector<uint8_t> packet;
		writeVarInt(packet, data.size());
		packet.insert(packet.end(), data.begin(), data.end());
		send(sock, packet.data(), packet.size(), 0);
	}

	std::vector<uint8_t> receivePacket(int sock)
	{
		int32_t length = readVarInt(sock);
		if (length <= 0)
			throw std::runtime_error("Invalid packet length");

		std::vector<uint8_t> buffer(length);
		int received = 0;
		while (received < length)
		{
			int ret = recv(sock, buffer.data() + received, length - received, 0);
			if (ret <= 0)
				throw std::runtime_error("Connection closed while receiving");
			received += ret;
		}
		return buffer;
	}
}
