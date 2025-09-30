#pragma once
#include "../World/UtilsPacket.hpp"
#include "../World/NBT.hpp"

#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace Courage::Network
{
	enum
	{
		REGISTRY_DATA_PACKET_ID = 0x07
	};

	class Registry
	{
	public:
		static void sendBiomeRegistry(int sock);
		static void sendWolfVariantRegistry(int sock);
		static void sendDamageTypeRegistry(int sock);
		static void sendTrimMaterialRegistry(int sock);
		static void sendTrimPatternRegistry(int sock);
		static void sendChatTypeRegistry(int sock);
		static void sendDimensionTypeRegistry(int sock);
		static void sendPaintingVariantRegistry(int sock);
	};
}