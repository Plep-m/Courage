#include "Courage/Network/Registry.hpp"

std::vector<uint8_t> buildDimensionPayloadFromValid(const std::string &dimensionName)
{
	return Courage::World::NBT::buildCompound([&](std::vector<uint8_t> &dim)
											  {
        if (dimensionName == "minecraft:overworld" || dimensionName == "minecraft:overworld_caves") {
            Courage::World::NBT::writeTAG_Byte(dim, "piglin_safe", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "natural", 1);
            Courage::World::NBT::writeTAG_Float(dim, "ambient_light", 0.0f);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_block_light_limit", 0);
            Courage::World::NBT::writeTAG_String(dim, "infiniburn", "#minecraft:infiniburn_overworld");
            Courage::World::NBT::writeTAG_Byte(dim, "respawn_anchor_works", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "has_skylight", 1);
            Courage::World::NBT::writeTAG_Byte(dim, "bed_works", 1);
            Courage::World::NBT::writeTAG_String(dim, "effects", "minecraft:overworld");
            Courage::World::NBT::writeTAG_Byte(dim, "has_raids", 1);
            Courage::World::NBT::writeTAG_Int(dim, "logical_height", 384);
            Courage::World::NBT::writeTAG_Double(dim, "coordinate_scale", 1.0);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_light_level", 7);
            Courage::World::NBT::writeTAG_String(dim, "type", "minecraft:uniform");
            Courage::World::NBT::writeTAG_Int(dim, "min_y", -64);
            Courage::World::NBT::writeTAG_Byte(dim, "ultrawarm", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "has_ceiling", (dimensionName == "minecraft:overworld_caves") ? 1 : 0);
            Courage::World::NBT::writeTAG_Int(dim, "height", 384);
        }
        else if (dimensionName == "minecraft:the_end") {
            Courage::World::NBT::writeTAG_Byte(dim, "piglin_safe", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "natural", 0);
            Courage::World::NBT::writeTAG_Float(dim, "ambient_light", 0.0f);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_block_light_limit", 0);
            Courage::World::NBT::writeTAG_String(dim, "infiniburn", "#minecraft:infiniburn_end");
            Courage::World::NBT::writeTAG_Byte(dim, "respawn_anchor_works", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "has_skylight", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "bed_works", 0);
            Courage::World::NBT::writeTAG_String(dim, "effects", "minecraft:the_end");
            Courage::World::NBT::writeTAG_Long(dim, "fixed_time", 6000L);
            Courage::World::NBT::writeTAG_Byte(dim, "has_raids", 1);
            Courage::World::NBT::writeTAG_Int(dim, "logical_height", 256);
            Courage::World::NBT::writeTAG_Double(dim, "coordinate_scale", 1.0);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_light_level", 7);
            Courage::World::NBT::writeTAG_String(dim, "type", "minecraft:uniform");
            Courage::World::NBT::writeTAG_Int(dim, "min_y", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "ultrawarm", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "has_ceiling", 0);
            Courage::World::NBT::writeTAG_Int(dim, "height", 256);
        }
        else if (dimensionName == "minecraft:the_nether") {
            Courage::World::NBT::writeTAG_Byte(dim, "piglin_safe", 1);
            Courage::World::NBT::writeTAG_Byte(dim, "natural", 1);
            Courage::World::NBT::writeTAG_Float(dim, "ambient_light", 0.1f);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_block_light_limit", 15);
            Courage::World::NBT::writeTAG_String(dim, "infiniburn", "#minecraft:infiniburn_nether");
            Courage::World::NBT::writeTAG_Byte(dim, "respawn_anchor_works", 1);
            Courage::World::NBT::writeTAG_Byte(dim, "has_skylight", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "bed_works", 0);
            Courage::World::NBT::writeTAG_String(dim, "effects", "minecraft:the_nether");
            Courage::World::NBT::writeTAG_Long(dim, "fixed_time", 18000L);
            Courage::World::NBT::writeTAG_Byte(dim, "has_raids", 0);
            Courage::World::NBT::writeTAG_Int(dim, "logical_height", 128);
            Courage::World::NBT::writeTAG_Double(dim, "coordinate_scale", 8.0);
            Courage::World::NBT::writeTAG_Int(dim, "monster_spawn_light_level", 7);
            Courage::World::NBT::writeTAG_Int(dim, "min_y", 0);
            Courage::World::NBT::writeTAG_Byte(dim, "ultrawarm", 1);
            Courage::World::NBT::writeTAG_Byte(dim, "has_ceiling", 1);
            Courage::World::NBT::writeTAG_Int(dim, "height", 256);
        } });
}

void Courage::Network::Registry::sendDimensionTypeRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:dimension_type");
	writeVarInt(packet, 4);

	// Entry 1: minecraft:overworld
	writeString(packet, "minecraft:overworld");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto overworldData = buildDimensionPayloadFromValid("minecraft:overworld");
	packet.insert(packet.end(), overworldData.begin(), overworldData.end());

	// Entry 2: minecraft:overworld_caves
	writeString(packet, "minecraft:overworld_caves");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto cavesData = buildDimensionPayloadFromValid("minecraft:overworld_caves");
	packet.insert(packet.end(), cavesData.begin(), cavesData.end());

	// Entry 3: minecraft:the_end
	writeString(packet, "minecraft:the_end");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto endData = buildDimensionPayloadFromValid("minecraft:the_end");
	packet.insert(packet.end(), endData.begin(), endData.end());

	// Entry 4: minecraft:the_nether
	writeString(packet, "minecraft:the_nether");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto netherData = buildDimensionPayloadFromValid("minecraft:the_nether");
	packet.insert(packet.end(), netherData.begin(), netherData.end());

	sendRawPacketBin(sock, packet);
}