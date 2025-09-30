#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendTrimMaterialRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:trim_material");
	writeVarInt(packet, 1);

	// 1. minecraft:iron
	writeString(packet, "minecraft:iron");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto ironData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													   {
        Courage::World::NBT::writeTAG_String(nbt, "asset_name", "iron");
        Courage::World::NBT::writeTAG_String(nbt, "ingredient", "minecraft:iron_ingot");
        Courage::World::NBT::writeTAG_Float(nbt, "item_model_index", 0.2f);
        
        auto overrideCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &override) {});
        Courage::World::NBT::writeTAG_Compound(nbt, "override_armor_materials", overrideCompound);
        
        auto descCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &desc) {
            Courage::World::NBT::writeTAG_String(desc, "translate", "trim_material.iron");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "description", descCompound); });
	packet.insert(packet.end(), ironData.begin(), ironData.end());

	sendRawPacketBin(sock, packet);
}