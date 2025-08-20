#include "Courage/Protocol/Status.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Config/Properties.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

using namespace Courage::Network;
using namespace Courage::Protocol;

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

inline std::string base64Encode(const std::vector<uint8_t> &bytes)
{
	std::string result;
	int val = 0, valb = -6;
	for (uint8_t c : bytes)
	{
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
		{
			result.push_back(base64_chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6)
		result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
	while (result.size() % 4)
		result.push_back('=');
	return result;
}

inline std::string encodeFileToBase64(const std::string &filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
		throw std::runtime_error("Invalid file : " + filename);

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return "data:image/png;base64," + base64Encode(buffer);
}

namespace Courage::Protocol
{
	void handleStatusRequest(int sock, const Properties& props)
	{
		auto request = receivePacket(sock, -1);
		if (request.empty() || request[0] != 0x00)
			throw std::runtime_error("Invalid status request");


		std::string motd = props.get("motd", "A Minecraft Server");
   		std::string versionName = props.get("version-name", "1.21");
		int protocol = props.getInt("protocol-version", 767);
		int maxPlayers = props.getInt("max-players", 20);

		std::string faviconPath = props.get("favicon", "../courage_favicon.png");
    	std::string faviconBase64 = encodeFileToBase64(faviconPath);

		std::string json = R"({
			"version": {"name": ")" + versionName + R"(", "protocol": )" + std::to_string(protocol) + R"(},
			"players": {"max": )" + std::to_string(maxPlayers) + R"(, "online": 0},
			"description": {"text": ")" + motd + R"("},
			"favicon": ")" + faviconBase64 + R"("
		})";

		std::vector<uint8_t> response;
		writeVarInt(response, 0x00);
		writeVarInt(response, json.size());
		response.insert(response.end(), json.begin(), json.end());

		sendPacket(sock, response, -1);

		auto ping = receivePacket(sock, -1);
		if (ping.empty() || ping[0] != 0x01)
			throw std::runtime_error("Invalid ping request");

		std::vector<uint8_t> pong;
		writeVarInt(pong, 0x01);
		pong.insert(pong.end(), ping.begin() + 1, ping.end());
		sendPacket(sock, pong, -1);
	}
}
