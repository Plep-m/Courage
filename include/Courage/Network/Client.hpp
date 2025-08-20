#pragma once

#include <string>
#include <mbedtls/md5.h>
#include <algorithm>
#include <array>

namespace Courage::Network
{

	class Client
	{
	public:
		Client(int socketFd, const std::string &username);
		~Client();

		struct Position
		{
			double x, y, z;
			float yaw, pitch;
			bool on_ground;
		};

		std::string getOfflineUUID() const { return offlineUUID_; }
		std::string getOnlineUUID() const { return onlineUUID_; }

		int getSocket() const { return socketFd_; }
		Position getPosition() const;
		std::string getUsername() const;
		int getEntityId() const;
		uint8_t getGamemode() const;
		std::array<uint8_t, 16> getUuid() const;

		void setPosition(double x, double y, double z, float yaw, float pitch);
		void setOnlineUUID(const std::string &uuid);
		void setGamemode(uint8_t mode);

		void sendPacket(const std::vector<uint8_t> &packet);
		void disconnect(const std::string &reason);

		bool isOnline() const;

	private:
		int socketFd_;
		std::string username_;

		std::string offlineUUID_;
		std::string onlineUUID_;

		int m_entity_id;

		Position m_position;
		uint8_t m_gamemode;
		bool m_online;

		void generateOfflineUUID(const std::string &username);
	};

}
