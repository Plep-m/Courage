#include "Courage/Network/Client.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/VarString.hpp"
#include <mbedtls/md5.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include <cstring>

namespace Courage::Network
{

	std::array<uint8_t, 16> Client::getUuid() const
	{
		std::array<uint8_t, 16> uuid{};
		std::string hex = offlineUUID_;
		
		hex.erase(std::remove(hex.begin(), hex.end(), '-'), hex.end());

		if (hex.size() != 32)
			throw std::runtime_error("Invalid UUID string length");

		for (size_t i = 0; i < 16; i++)
		{
			std::string byteStr = hex.substr(i * 2, 2);
			uuid[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
		}

		return uuid;
	}

	Client::Client(int socketFd, const std::string &username)
		: socketFd_(socketFd), username_(username), m_gamemode(1), m_online(true)
	{
		generateOfflineUUID(username_);
		setPosition(0.0, 64.0, 0.0, 0.0f, 0.0f);
	}

	Client::~Client()
	{
		if (socketFd_ > 0)
		{
			::close(socketFd_);
			disconnect("Player object destroyed");
		}
	}

	void Client::sendPacket(const std::vector<uint8_t> &packet)
	{
		if (m_online)
			Network::sendPacket(socketFd_, packet, -1);
	}

	void Client::disconnect(const std::string &reason)
	{
		if (!m_online)
			return;

		std::vector<uint8_t> packet;
		Protocol::writeVarInt(packet, 0x00); // Disconnect packet ID

		std::string json_reason = R"({"text":")" + reason + R"("})";
		Protocol::writeVarString(packet, json_reason);

		sendPacket(packet);
		m_online = false;
	}

	void Client::setPosition(double x, double y, double z, float yaw, float pitch)
	{
		m_position = {x, y, z, yaw, pitch, true};
	}

	Client::Position Client::getPosition() const
	{
		return m_position;
	}

	std::string Client::getUsername() const
	{
		return username_;
	}

	int Client::getEntityId() const
	{
		return m_entity_id;
	}

	bool Client::isOnline() const
	{
		return m_online;
	}

	void Client::setGamemode(uint8_t mode)
	{
		m_gamemode = mode;
	}

	uint8_t Client::getGamemode() const
	{
		return m_gamemode;
	}

	void Client::generateOfflineUUID(const std::string &username)
	{
		std::string base = "OfflinePlayer:" + username;

		unsigned char md5sum[16];

		mbedtls_md5_context ctx;
		mbedtls_md5_init(&ctx);
		mbedtls_md5_starts_ret(&ctx);
		mbedtls_md5_update_ret(&ctx, reinterpret_cast<const unsigned char *>(base.data()), base.size());
		mbedtls_md5_finish_ret(&ctx, md5sum);
		mbedtls_md5_free(&ctx);

		md5sum[6] = (md5sum[6] & 0x0F) | 0x30;
		md5sum[8] = (md5sum[8] & 0x3F) | 0x80;

		uuid_t binuuid;
		std::memcpy(binuuid, md5sum, 16);

		char uuidStr[37];
		uuid_unparse(binuuid, uuidStr);

		offlineUUID_ = uuidStr;
	}

	void Client::setOnlineUUID(const std::string &uuid)
	{
		onlineUUID_ = uuid;
	}
}
