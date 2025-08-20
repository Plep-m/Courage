#include "Courage/Network/LoginHandler.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/VarString.hpp"
#include "Courage/Protocol/VarDouble.hpp"
#include "Courage/Protocol/VarFloat.hpp"
#include "Courage/Protocol/Status.hpp"
#include <stdexcept>

#include <iostream>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

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

namespace Courage::Network
{
	LoginHandler::LoginHandler() {}

	LoginHandler::~LoginHandler()
	{
		for (auto &pair : m_players)
			pair.second->disconnect("Server shutdown");
		m_players.clear();
	}

	void LoginHandler::handleConnection(int sock, const Properties &props)
	{
		try
		{
			handleHandshake(sock, props);
		}
		catch (const std::exception &e)
		{
			::close(sock);
		}
	}

	void LoginHandler::handleHandshake(int sock, const Properties &props)
	{
		auto handshake = Network::receivePacket(sock, -1);
		if (handshake.empty())
			throw std::runtime_error("Empty handshake packet");

		size_t pos = 0;
		int32_t packetId = Protocol::readVarInt(handshake, pos);

		if (packetId != 0x00)
			throw std::runtime_error("Expected Handshake (0x00), got: " + std::to_string(packetId));

		int32_t protocolVersion = Protocol::readVarInt(handshake, pos);
		std::string serverAddress = Protocol::readVarString(handshake, pos);
		uint16_t serverPort = Protocol::readVarInt(handshake, pos);
		int32_t nextState = Protocol::readVarInt(handshake, pos);
		if (handshake.back() == 1)
		{
			sendServerIdentification(sock, props);
			Protocol::handleStatusRequest(sock, props);
		}
		else if (handshake.back() == 2)
		{
			sendServerIdentification(sock, props);
			handleLoginStart(sock);
		}
		else
			throw std::runtime_error("Unexpected next state: " + std::to_string(nextState));
	}

	void LoginHandler::handleLoginStart(int sock)
	{
		auto loginStart = Network::receivePacket(sock, -1);
		size_t pos = 0;
		int32_t packetId = Protocol::readVarInt(loginStart, pos);

		if (packetId != 0x00)
			throw std::runtime_error("Expected Login Start (0x00), got: " + std::to_string(packetId));

		std::string username = Protocol::readVarString(loginStart, pos);

		auto player = std::make_shared<Client>(sock, username);
		m_players[sock] = player;

		sendLoginSuccess(sock, username);

		sendJoinGame(sock, player);
		sendPlayerPosition(sock, player);
	}

	void LoginHandler::sendServerIdentification(int sock, const Properties &props)
	{
		auto request = Network::receivePacket(sock, -1);
		if (request.empty() || request[0] != 0x00)
			throw std::runtime_error("Invalid status request");

		std::string motd = props.get("motd", "A Minecraft Server");
		std::string versionName = props.get("version-name", "1.21");
		int protocol = props.getInt("protocol-version", 767);
		int maxPlayers = props.getInt("max-players", 20);

		std::string faviconPath = props.get("favicon", "../courage_favicon.png");
		std::string faviconBase64 = encodeFileToBase64(faviconPath);

		std::string json = R"({"version":{"name":")" + versionName +
						   R"(","protocol":)" + std::to_string(protocol) +
						   R"(},"players":{"max":)" + std::to_string(maxPlayers) +
						   R"(,"online":0},"description":{"text":")" + motd +
						   R"("},"favicon":")" + faviconBase64 + R"("})";

		std::vector<uint8_t> response;
		Protocol::writeVarInt(response, 0x00);
		Protocol::writeVarString(response, json);

		Network::sendPacket(sock, response, -1);
	}

	void LoginHandler::sendLoginSuccess(int sock, const std::string &username)
	{
		auto player = getPlayer(sock);
		if (!player)
			throw std::runtime_error("Player not found for socket: " + std::to_string(sock));

		std::vector<uint8_t> packet;
		Protocol::writeVarInt(packet, 0x02);

		auto uuid = player->getUuid();
		for (int i = 0; i < 16; i++)
			packet.push_back(uuid[i]);

		Protocol::writeVarString(packet, username);

		Protocol::writeVarInt(packet, 0);

		Network::sendPacket(sock, packet, -1);
	}

	void LoginHandler::sendJoinGame(int sock, std::shared_ptr<Client> player)
	{
		std::vector<uint8_t> packet;
		Protocol::writeVarInt(packet, 0x28);
		Protocol::writeVarInt(packet, player->getEntityId());
		packet.push_back(player->getGamemode());
		packet.push_back(0);
		packet.push_back(0);
		packet.push_back(0);
		Protocol::writeVarString(packet, "default");
		packet.push_back(0);
		Network::sendPacket(sock, packet, -1);
	}

	void LoginHandler::sendPlayerPosition(int sock, std::shared_ptr<Client> player)
	{
		auto pos = player->getPosition();

		std::vector<uint8_t> packet;
		Protocol::writeVarInt(packet, 0x38);
		Protocol::writeDouble(packet, pos.x);
		Protocol::writeDouble(packet, pos.y);
		Protocol::writeDouble(packet, pos.z);

		Protocol::writeFloat(packet, pos.yaw);
		Protocol::writeFloat(packet, pos.pitch);

		packet.push_back(0x00);

		Network::sendPacket(sock, packet, -1);
	}

	std::shared_ptr<Client> LoginHandler::getPlayer(int sock) const
	{
		auto it = m_players.find(sock);
		if (it != m_players.end())
			return it->second;
		return nullptr;
	}

	void LoginHandler::removePlayer(int sock)
	{
		auto it = m_players.find(sock);
		if (it != m_players.end())
			m_players.erase(it);
	}
}