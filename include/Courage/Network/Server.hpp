#pragma once
#include <string>

namespace Courage::Network
{
	class Server
	{
	public:
		Server(int port);
		~Server();
		void start();

	private:
		int port;
		int server_fd;
	};
}
