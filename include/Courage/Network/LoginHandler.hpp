#pragma once

#include "Client.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Config/Properties.hpp"
#include <memory>
#include <unordered_map>

namespace Courage::Network
{

	class LoginHandler
	{
	public:
		LoginHandler();
		~LoginHandler();

		void handleConnection(int sock, const Properties& props);

		std::shared_ptr<Client> getPlayer(int sock) const;
		void removePlayer(int sock);

	private:
		void handleHandshake(int sock, const Properties& props);
		void handleLoginStart(int sock);
		void sendLoginSuccess(int sock);
		void waitForLoginAck(int sock);
		void sendConfigurationPackets(int sock);
		void sendBrandPacket(int sock);
		void waitForFinishConfig(int sock);
		void sendGamePackets(int sock);
		void sendJoinGame(int sock);
		void sendPlayerInfo(int sock);
		void sendWorldBorder(int sock);
		void sendSpawnPosition(int sock);
		void sendPlayerAbilities(int sock);
		void sendHealth(int sock);
		void sendServerIdentification(int sock, const Properties &props);
		void sendKnownPacks(int sock);
		void sendRegistryData(int sock);
		void sendTags(int sock);
		void sendFinishConfiguration(int sock);
		void sendDifficulty(int sock);
		void sendHeldItemSlot(int sock);
		void sendTimeUpdate(int sock);
		void sendChunkData(int sock);
		void sendFeatureFlags(int sock);
		void sendCompression(int sock);
		void sendRawPacket(int socket, const std::string &hex);
		void sendPlayerPosition(int sock);
		void sendGameStateChange(int sock);
		void sendStepTick(int sock);
		void sendUpdateViewPosition(int sock);
		void sendBundleDelimiter(int sock);
		void sendExperience(int sock);
		void sendBinaryPacket(int socket, const std::vector<uint8_t> &data);

		std::unordered_map<int, std::shared_ptr<Client>> m_players;
	};
}