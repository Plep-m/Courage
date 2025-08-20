#pragma once
#include <string>
#include "Courage/Config/Properties.hpp"

namespace Courage::Protocol
{
	void handleStatusRequest(int sock, const Properties& props);
	void handleLoginRequest(int sock);
}
