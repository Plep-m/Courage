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
		void sendServerIdentification(int sock, const Properties &props);
		void sendLoginSuccess(int sock, const std::string &username);
		void sendJoinGame(int sock, std::shared_ptr<Client> player);
		void sendPlayerPosition(int sock, std::shared_ptr<Client> player);

		std::unordered_map<int, std::shared_ptr<Client>> m_players;
	};
}