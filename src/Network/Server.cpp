#include "Courage/Network/Server.hpp"
#include "Courage/Network/PacketIO.hpp"
#include "Courage/Protocol/VarInt.hpp"
#include "Courage/Protocol/Status.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Courage::Protocol;

namespace Courage::Network
{

	Server::~Server()
	{
		if (server_fd >= 0)
		{
			close(server_fd);
			std::cout << "[INFO] Serveur arrêté et socket fermé." << std::endl;
		}
	}

	Server::Server(int port, const Properties& props) : port(port), props(props)
	{
		server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd < 0)
			throw std::runtime_error("Impossible de créer le socket");
		
		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Erreur setsockopt()");

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);

		if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
			throw std::runtime_error("Erreur bind()");

		if (listen(server_fd, 5) < 0)
			throw std::runtime_error("Erreur listen()");
	}

	void Server::start()
	{
		std::cout << "[INFO] Serveur démarré sur le port " << port << std::endl;

		while (true)
		{
			sockaddr_in client_addr{};
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);
			if (client_fd < 0)
				continue;

			try
			{
				auto handshake = receivePacket(client_fd, -1);
				if (handshake.empty() || handshake[0] != 0x00)
					throw std::runtime_error("Invalid handshake packet");

				int next_state = handshake.back();
				if (next_state == 1)
					handleStatusRequest(client_fd, props);
				else if (next_state == 2)
					handleLoginRequest(client_fd);
			}
			catch (std::exception &e)
			{
				std::cerr << "[ERREUR] " << e.what() << std::endl;
			}

			close(client_fd);
		}
	}
}
