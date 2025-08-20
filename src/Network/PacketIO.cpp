#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/utils.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <cstring>

using namespace Courage::Protocol;

namespace Courage::Network
{

	void sendPacket(int sock, const std::vector<uint8_t> &data, int compressionThreshold)
	{
		std::vector<uint8_t> packet;

		if (compressionThreshold > 0 && data.size() >= (size_t)compressionThreshold)
		{
			std::string input(reinterpret_cast<const char *>(data.data()), data.size());
			std::string compressed;
			Courage::compressString(input, compressed);
			writeVarInt(packet, data.size());
			packet.insert(packet.end(), compressed.begin(), compressed.end());
		}
		else if (compressionThreshold > 0)
		{
			writeVarInt(packet, 0);
			packet.insert(packet.end(), data.begin(), data.end());
		}
		else
			packet.insert(packet.end(), data.begin(), data.end());

		std::vector<uint8_t> finalPacket;
		writeVarInt(finalPacket, packet.size());
		finalPacket.insert(finalPacket.end(), packet.begin(), packet.end());
		send(sock, finalPacket.data(), finalPacket.size(), 0);
	}

	std::vector<uint8_t> receivePacket(int sock, int compressionThreshold)
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

		if (compressionThreshold > 0)
		{
			size_t index = 0;
			int32_t uncompressedSize = readVarInt(buffer, index);

			if (uncompressedSize != 0)
			{
				std::string compressed(reinterpret_cast<const char *>(buffer.data() + index), buffer.size() - index);
				std::string decompressed;
				Courage::decompressString(compressed, decompressed);
				return std::vector<uint8_t>(decompressed.begin(), decompressed.end());
			}
			else
				return std::vector<uint8_t>(buffer.begin() + index, buffer.end());
		}
		else
			return buffer;
	}
}
