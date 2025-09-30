#include "Courage/World/UtilsPacket.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include <sys/socket.h>

void writeVarLong(std::vector<uint8_t> &buffer, int64_t value)
{
	while (true)
	{
		if ((value & ~0x7FLL) == 0)
		{
			buffer.push_back(static_cast<uint8_t>(value));
			return;
		}
		buffer.push_back(static_cast<uint8_t>((value & 0x7F) | 0x80));
		value >>= 7;
	}
}

void writeVarInt(std::vector<uint8_t> &buffer, int32_t value)
{
	while (true)
	{
		if ((value & ~0x7F) == 0)
		{
			buffer.push_back(static_cast<uint8_t>(value));
			return;
		}
		buffer.push_back(static_cast<uint8_t>((value & 0x7F) | 0x80));
		value >>= 7;
	}
}

void writeInt32(std::vector<uint8_t> &buffer, int32_t value)
{
	buffer.push_back(static_cast<uint8_t>(value >> 24));
	buffer.push_back(static_cast<uint8_t>(value >> 16));
	buffer.push_back(static_cast<uint8_t>(value >> 8));
	buffer.push_back(static_cast<uint8_t>(value));
}

void writeByte(std::vector<uint8_t> &buffer, uint8_t value)
{
	buffer.push_back(value);
}

void writeBool(std::vector<uint8_t> &buffer, bool value)
{
	buffer.push_back(value ? 0x01 : 0x00);
}

void writeLongArray(std::vector<uint8_t> &buffer, const std::vector<int64_t> &array)
{
	writeInt32(buffer, array.size());
	for (int64_t val : array)
		writeInt64(buffer, val);
}

void writeByteArray(std::vector<uint8_t> &buffer, const std::vector<uint8_t> &data)
{
	writeVarInt(buffer, static_cast<int32_t>(data.size()));
	buffer.insert(buffer.end(), data.begin(), data.end());
}

void writeInt64(std::vector<uint8_t> &buffer, int64_t value)
{
	for (int i = 7; i >= 0; i--)
		buffer.push_back(static_cast<uint8_t>(value >> (8 * i)));
}

void writeInt16(std::vector<uint8_t> &buffer, uint16_t value)
{
	buffer.push_back(static_cast<uint8_t>(value >> 8));
	buffer.push_back(static_cast<uint8_t>(value));
}

void writeString(std::vector<uint8_t> &buffer, const std::string &str)
{
	writeVarInt(buffer, static_cast<int32_t>(str.length()));

	const char *cstr = str.c_str();
	for (size_t i = 0; i < str.length(); ++i)
		buffer.push_back(static_cast<uint8_t>(cstr[i]));
}

void writeLong(std::vector<uint8_t> &buffer, int64_t value)
{
	for (int i = 7; i >= 0; i--)
		buffer.push_back((value >> (i * 8)) & 0xFF);
}

void writeBytes(std::vector<uint8_t> &out, const std::vector<uint8_t> &src)
{
	out.insert(out.end(), src.begin(), src.end());
}

void writeShort(std::vector<uint8_t> &out, int16_t value)
{
	out.push_back((value >> 8) & 0xFF);
	out.push_back(value & 0xFF);
}

void writeStringRaw(std::vector<uint8_t> &out, const std::string &str)
{
	out.insert(out.end(), str.begin(), str.end());
}

inline std::vector<uint8_t> hexToBytes(const std::string &hex)
{
	std::vector<uint8_t> bytes;
	for (size_t i = 0; i < hex.length(); i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}
	return bytes;
}

void sendRawPacket(int socket, const std::string &hex)
{
	auto bytes = hexToBytes(hex);

	std::vector<uint8_t> packet;
	writeVarInt(packet, bytes.size());
	packet.insert(packet.end(), bytes.begin(), bytes.end());

	send(socket, packet.data(), packet.size(), 0);
}

void sendRawPacketBin(int socket, const std::vector<uint8_t> &data)
{
	std::vector<uint8_t> framed;
	writeVarInt(framed, static_cast<int32_t>(data.size()));
	framed.insert(framed.end(), data.begin(), data.end());
	send(socket, framed.data(), framed.size(), 0);
}