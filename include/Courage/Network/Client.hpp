#pragma once

#include <string>
#include <mbedtls/md5.h>
#include <algorithm>
#include <array>

namespace Courage::Network
{

	enum class ConnectionState
	{
		HANDSHAKE,
		STATUS,
		LOGIN,
		CONFIGURATION,
		PLAY
	};

	class Client
	{
	public:
		Client(int socketFd);
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
		ConnectionState getConnectionState();
		float getHealth();
		int getFood();
		float getSaturation();

		void setPosition(double x, double y, double z, float yaw, float pitch);
		void setOnlineUUID(const std::string &uuid);
		void setGamemode(uint8_t mode);
		void setUsername(const std::string &username);
		void setConnectionState(ConnectionState state);
		void setHealth(float health);
		void setFood(int food);
		void setSaturation(float saturation);

		void sendPacket(const std::vector<uint8_t> &packet);
		void disconnect(const std::string &reason);

		bool isOnline() const;

	private:
		int socketFd_;
		std::string username_;

		std::string offlineUUID_;
		std::string onlineUUID_;

		int entity_id_;

		Position position_;
		uint8_t gamemode_;
		bool online_;
		ConnectionState connectionState_;

		float health_;
		int food_;
		float saturation_;

		void generateOfflineUUID(const std::string &username);
	};

}
