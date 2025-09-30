#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendTrimPatternRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:trim_pattern");
	writeVarInt(packet, 1);

	// 1. minecraft:sentry
	writeString(packet, "minecraft:sentry");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto sentryData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        Courage::World::NBT::writeTAG_String(nbt, "asset_id", "minecraft:sentry");
        Courage::World::NBT::writeTAG_String(nbt, "template_item", "minecraft:sentry_armor_trim_smithing_template");

        auto descCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &desc) {
            Courage::World::NBT::writeTAG_String(desc, "translate", "trim_pattern.sentry");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "description", descCompound);
        
        Courage::World::NBT::writeTAG_Byte(nbt, "decal", 0); });
	packet.insert(packet.end(), sentryData.begin(), sentryData.end());

	sendRawPacketBin(sock, packet);
}