#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendWolfVariantRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:wolf_variant");
	writeVarInt(packet, 1);

	writeString(packet, "minecraft:pale");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto paleData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													   {
        Courage::World::NBT::writeTAG_String(nbt, "wild_texture", "minecraft:entity/wolf/wolf");
        Courage::World::NBT::writeTAG_String(nbt, "tame_texture", "minecraft:entity/wolf/wolf_tame");
        Courage::World::NBT::writeTAG_String(nbt, "angry_texture", "minecraft:entity/wolf/wolf_angry");
        
        std::vector<std::string> biomes = {"minecraft:taiga"};
        Courage::World::NBT::writeTAG_List_of_String(nbt, "biomes", biomes); });
	packet.insert(packet.end(), paleData.begin(), paleData.end());

	sendRawPacketBin(sock, packet);
}