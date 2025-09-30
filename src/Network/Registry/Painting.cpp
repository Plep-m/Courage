#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendPaintingVariantRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:painting_variant");
	writeVarInt(packet, 1);

	writeString(packet, "minecraft:kebab");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto kebabData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
        Courage::World::NBT::writeTAG_String(nbt, "asset_id", "minecraft:kebab");
        Courage::World::NBT::writeTAG_Int(nbt, "width", 1);
        Courage::World::NBT::writeTAG_Int(nbt, "height", 1);
        Courage::World::NBT::writeTAG_String(nbt, "title", "Kebab");
        Courage::World::NBT::writeTAG_String(nbt, "author", "Kristoffer Zetterstrand"); });
	packet.insert(packet.end(), kebabData.begin(), kebabData.end());

	sendRawPacketBin(sock, packet);
}