#pragma once

#include "Courage/Network/Client.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Network/Registry.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/VarString.hpp"
#include "Courage/Protocol/Status.hpp"
#include "Courage/Network/PacketBuilder.hpp"
#include "Courage/Logic/Experience.hpp"
#include "Courage/World/UtilsPacket.hpp"
#include "Courage/Network/Chunk.hpp"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <unordered_map>

namespace Courage::Network
{

	class LoginHandler
	{
	public:
		LoginHandler();
		~LoginHandler();

		void handleConnection(int sock, const Properties &props);

		std::shared_ptr<Client> getPlayer(int sock) const;
		void removePlayer(int sock);

	private:
		void handleHandshake(int sock, const Properties &props);
		void handleLoginStart(int sock);
		void sendLoginSuccess(int sock);
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
		void sendFinishConfiguration(int sock);
		void sendDifficulty(int sock);
		void sendHeldItemSlot(int sock);
		void sendTimeUpdate(int sock);
		void sendChunkData(int sock);
		void sendFeatureFlags(int sock);
		void sendPlayerPosition(int sock);
		void sendGameStateChange(int sock);
		void sendBundleDelimiter(int sock);
		void sendExperience(int sock);
		int sendChunksAroundPlayer(int sock, int playerChunkX, int playerChunkZ, int viewDistance);

		std::unordered_map<int, std::shared_ptr<Client>> m_players;
	};
}