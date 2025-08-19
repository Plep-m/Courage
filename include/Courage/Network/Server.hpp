#pragma once
#include <string>
#include "Courage/Config/Properties.hpp"

namespace Courage::Network
{
	class Server
	{
	public:
		Server(int port, const Properties& props);
		~Server();
		void start();

	private:
		int port;
		int server_fd;
		const Properties& props;
	};
}
