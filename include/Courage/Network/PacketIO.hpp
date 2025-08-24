#pragma once
#include <string>
#include <vector>
#include <stdint.h>

namespace Courage::Network
{
	namespace PacketIds
	{
		constexpr int HANDSHAKE = 0x00;
		constexpr int LOGIN_START = 0x00;
		constexpr int STATUS = 0x01;
		constexpr int LOGIN_SUCCESS = 0x02;
		constexpr int LOGIN_ACKNOWLEDGMENT = 0x03;
		constexpr int JOIN_GAME = 0x26;
		constexpr int PLAYER_POSITION = 0x38;
		constexpr int PLAYER_INFO = 0x36;
		constexpr int DIFFICULTY = 0x0E;
		constexpr int PLAYER_ABILITIES = 0x32;
		constexpr int HELD_ITEM_CHANGE = 0x4A;
		constexpr int SPAWN_POSITION = 0x4D;
		constexpr int TIME_UPDATE = 0x5E;
		constexpr int UPDATE_VIEW_POSITION = 0x49;
		constexpr int CHUNK_DATA = 0x22;
		constexpr int PLUGIN_MESSAGE = 0x18;
		constexpr int FINISH_CONFIGURATION = 0x02;
		constexpr int DATA_REGISTRY = 0x05;
		constexpr int FEATURE_FLAG = 0x08;
		constexpr int ACKNOWLEDGE_CONFIGURATION = 0x00;
	}

	void sendPacket(int sock, const std::vector<uint8_t> &data, int compressionThreshold);
	std::vector<uint8_t> receivePacket(int sock, int compressionThreshold);
}
