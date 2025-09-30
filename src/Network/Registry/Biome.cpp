#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendBiomeRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:worldgen/biome");
	writeVarInt(packet, 4);

	// ========== 1. PLAINS ==========
	writeString(packet, "minecraft:plains");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto plainsData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        Courage::World::NBT::writeTAG_Byte(nbt, "has_precipitation", 1);
        Courage::World::NBT::writeTAG_Float(nbt, "temperature", 0.8f);
        Courage::World::NBT::writeTAG_Float(nbt, "downfall", 0.4f);
        Courage::World::NBT::writeTAG_String(nbt, "temperature_modifier", "none");
        Courage::World::NBT::writeTAG_String(nbt, "category", "plains");

        auto effectsCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &effects) {
            Courage::World::NBT::writeTAG_Int(effects, "sky_color", 7907327);
            Courage::World::NBT::writeTAG_Int(effects, "water_color", 4159204);
            Courage::World::NBT::writeTAG_Int(effects, "water_fog_color", 329011);
            Courage::World::NBT::writeTAG_Int(effects, "fog_color", 12638463);
            Courage::World::NBT::writeTAG_String(effects, "grass_color_modifier", "none");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "effects", effectsCompound); });
	packet.insert(packet.end(), plainsData.begin(), plainsData.end());

	// ========== 2. FOREST ==========
	writeString(packet, "minecraft:forest");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto forestData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        Courage::World::NBT::writeTAG_Byte(nbt, "has_precipitation", 1);
        Courage::World::NBT::writeTAG_Float(nbt, "temperature", 0.7f);
        Courage::World::NBT::writeTAG_Float(nbt, "downfall", 0.8f);
        Courage::World::NBT::writeTAG_String(nbt, "temperature_modifier", "none");
        Courage::World::NBT::writeTAG_String(nbt, "category", "forest");

        auto effectsCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &effects) {
            Courage::World::NBT::writeTAG_Int(effects, "sky_color", 7972607);
            Courage::World::NBT::writeTAG_Int(effects, "water_color", 4159204);
            Courage::World::NBT::writeTAG_Int(effects, "water_fog_color", 329011);
            Courage::World::NBT::writeTAG_Int(effects, "fog_color", 12638463);
            Courage::World::NBT::writeTAG_String(effects, "grass_color_modifier", "none");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "effects", effectsCompound); });
	packet.insert(packet.end(), forestData.begin(), forestData.end());

	// ========== 3. DESERT ==========
	writeString(packet, "minecraft:desert");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto desertData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        Courage::World::NBT::writeTAG_Byte(nbt, "has_precipitation", 0);
        Courage::World::NBT::writeTAG_Float(nbt, "temperature", 2.0f);
        Courage::World::NBT::writeTAG_Float(nbt, "downfall", 0.0f);
        Courage::World::NBT::writeTAG_String(nbt, "temperature_modifier", "none");
        Courage::World::NBT::writeTAG_String(nbt, "category", "desert");

        auto effectsCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &effects) {
            Courage::World::NBT::writeTAG_Int(effects, "sky_color", 7254527);
            Courage::World::NBT::writeTAG_Int(effects, "water_color", 4159204);
            Courage::World::NBT::writeTAG_Int(effects, "water_fog_color", 329011);
            Courage::World::NBT::writeTAG_Int(effects, "fog_color", 12638463);
            Courage::World::NBT::writeTAG_String(effects, "grass_color_modifier", "none");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "effects", effectsCompound); });
	packet.insert(packet.end(), desertData.begin(), desertData.end());

	// ========== 4. TAIGA ==========
	writeString(packet, "minecraft:taiga");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto taigaData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
        Courage::World::NBT::writeTAG_Byte(nbt, "has_precipitation", 1);
        Courage::World::NBT::writeTAG_Float(nbt, "temperature", 0.25f);
        Courage::World::NBT::writeTAG_Float(nbt, "downfall", 0.8f);
        Courage::World::NBT::writeTAG_String(nbt, "temperature_modifier", "none");
        Courage::World::NBT::writeTAG_String(nbt, "category", "taiga");

        auto effectsCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &effects) {
            Courage::World::NBT::writeTAG_Int(effects, "sky_color", 8233727);
            Courage::World::NBT::writeTAG_Int(effects, "water_color", 4020182);
            Courage::World::NBT::writeTAG_Int(effects, "water_fog_color", 329011);
            Courage::World::NBT::writeTAG_Int(effects, "fog_color", 12638463);
            Courage::World::NBT::writeTAG_String(effects, "grass_color_modifier", "none");
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "effects", effectsCompound); });
	packet.insert(packet.end(), taigaData.begin(), taigaData.end());

	sendRawPacketBin(sock, packet);
}