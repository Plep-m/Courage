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

using namespace Courage::Network;
namespace Courage::Protocol

{
	void handleLoginRequest(const NetEvent& ev) {
        const auto& data = *ev.payload;
        if (data.empty() || data[0] != 0x00)
            throw std::runtime_error("Invalid login start packet");

        size_t pos = 1;
        std::string username = readVarString(data, pos);

        Client client(ev.clientFd, username);
        std::string offlineUUID = client.getOfflineUUID();

        DEBUG_LOG("Generated offline UUID for user '" << username << "': " << offlineUUID);

        // Compression
        std::vector<uint8_t> setCompression;
        writeVarInt(setCompression, 0x03);
        writeVarInt(setCompression, 256);
        sendPacket(ev.clientFd, setCompression, -1);

        // Login success
        std::vector<uint8_t> loginSuccess;
        writeVarInt(loginSuccess, 0x02);
        writeVarString(loginSuccess, offlineUUID);
        writeVarString(loginSuccess, username);
        loginSuccess.push_back(0x00); // hasProperties=false
        sendPacket(ev.clientFd, loginSuccess, 256);

        // At this point, switch the connection state to "Play".
    }
}
