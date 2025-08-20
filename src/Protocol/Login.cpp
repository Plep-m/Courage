#include "Courage/Protocol/Status.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/VarString.hpp"
#include "Courage/Config/Properties.hpp"
#include "Courage/Network/Client.hpp"
#include "Courage/utils.hpp"

#include <sys/socket.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

namespace Courage::Protocol
{
	void handleLoginRequest(int sock)
	{
		auto loginStart = Network::receivePacket(sock, -1);
		if (loginStart.empty() || loginStart[0] != 0x00)
			throw std::runtime_error("Invalid login start packet");

		size_t pos = 1;
		std::string username = readVarString(loginStart, pos);

		Courage::Network::Client client(sock, username);
		std::string offlineUUID = client.getOfflineUUID();

		DEBUG_LOG("Generated offline UUID for user '" << username << "': " << offlineUUID);

		std::vector<uint8_t> setCompression;
		writeVarInt(setCompression, 0x03);
		writeVarInt(setCompression, 256);
		Network::sendPacket(sock, setCompression, -1);

		std::vector<uint8_t> loginSuccess;
		writeVarInt(loginSuccess, 0x02);

		writeVarString(loginSuccess, offlineUUID);
		writeVarString(loginSuccess, username);
		loginSuccess.push_back(0x00); // TODO: désactiver les hasProperties (A changer plus tard) = skin, cape etc
		Network::sendPacket(sock, loginSuccess, 256);
		// TODO: Charger le jeu j'imagine
	}
}
