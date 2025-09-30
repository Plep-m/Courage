#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendDamageTypeRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:damage_type");
	writeVarInt(packet, 25);

	// 1. minecraft:generic
	writeString(packet, "minecraft:generic");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto genericData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														  {
        Courage::World::NBT::writeTAG_String(nbt, "message_id", "generic");
        Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
        Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), genericData.begin(), genericData.end());

	// 2. minecraft:player_attack
	writeString(packet, "minecraft:player_attack");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto playerAttackData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															   {
        Courage::World::NBT::writeTAG_String(nbt, "message_id", "player");
        Courage::World::NBT::writeTAG_String(nbt, "scaling", "when_caused_by_living_non_player");
        Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.1f); });
	packet.insert(packet.end(), playerAttackData.begin(), playerAttackData.end());

	// 3. minecraft:in_fire
	writeString(packet, "minecraft:in_fire");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto inFireData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
			Courage::World::NBT::writeTAG_String(nbt, "message_id", "inFire");
			Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.1f);
			Courage::World::NBT::writeTAG_String(nbt, "scaling", "when_caused_by_living_non_player");
			Courage::World::NBT::writeTAG_String(nbt, "effects", "burning"); });
	packet.insert(packet.end(), inFireData.begin(), inFireData.end());

	// 4. minecraft:campfire
	writeString(packet, "minecraft:campfire");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto campfireData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "campfire");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), campfireData.begin(), campfireData.end());

	// 5. minecraft:lightning_bolt
	writeString(packet, "minecraft:lightning_bolt");
	packet.push_back(0x01);
	packet.push_back(0x0A);

	auto lightningData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															{
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "lightningBolt");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), lightningData.begin(), lightningData.end());

	// 6. minecraft:on_fire
	writeString(packet, "minecraft:on_fire");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto onFireData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "onFire");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), onFireData.begin(), onFireData.end());

	// 7. minecraft:lava
	writeString(packet, "minecraft:lava");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto lavaData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "lava");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), lavaData.begin(), lavaData.end());

	// 8. minecraft:hot_floor
	writeString(packet, "minecraft:hot_floor");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto hotFloorData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "hotFloor");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), hotFloorData.begin(), hotFloorData.end());

	// 9. minecraft:in_wall
	writeString(packet, "minecraft:in_wall");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto inWallData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "in_wall");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), inWallData.begin(), inWallData.end());

	// 10. minecraft:cramming
	writeString(packet, "minecraft:cramming");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto crammingData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "cramming");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), crammingData.begin(), crammingData.end());

	// 11. minecraft:drown
	writeString(packet, "minecraft:drown");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto drownData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "drown");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), drownData.begin(), drownData.end());

	// 12. minecraft:starve
	writeString(packet, "minecraft:starve");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto starveData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "starve");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), starveData.begin(), starveData.end());

	// 13. minecraft:cactus
	writeString(packet, "minecraft:cactus");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto cactusData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "cactus");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), cactusData.begin(), cactusData.end());

	// 14. minecraft:fall
	writeString(packet, "minecraft:fall");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto fallDaTa = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "fall");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), fallDaTa.begin(), fallDaTa.end());

	// 15. minecraft:fly_into_wall
	writeString(packet, "minecraft:fly_into_wall");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto flyIntoWallData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															  {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "fly_into_wall");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), flyIntoWallData.begin(), flyIntoWallData.end());

	// 16. minecraft:out_of_world
	writeString(packet, "minecraft:out_of_world");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto outOfWorldData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "out_of_world");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), outOfWorldData.begin(), outOfWorldData.end());

	// 17. minecraft:magic
	writeString(packet, "minecraft:magic");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto magicData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "magic");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), magicData.begin(), magicData.end());

	// 18. minecraft:wither
	writeString(packet, "minecraft:wither");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto wither = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "wither");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), wither.begin(), wither.end());

	// 19. minecraft:dragon_breath
	writeString(packet, "minecraft:dragon_breath");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto dragonBreathData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															   {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "dragon_breath");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), dragonBreathData.begin(), dragonBreathData.end());

	// 20. minecraft:dry_out
	writeString(packet, "minecraft:dry_out");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto dryOutData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "dry_out");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), dryOutData.begin(), dryOutData.end());

	// 21. minecraft:sweet_berry_bush
	writeString(packet, "minecraft:sweet_berry_bush");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto SweetBerryBushData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
																 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "sweet_berry_bush");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), SweetBerryBushData.begin(), SweetBerryBushData.end());

	// 22. minecraft:freeze
	writeString(packet, "minecraft:freeze");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto freezeData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "freeze");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), freezeData.begin(), freezeData.end());

	// 23. minecraft:stalagmite
	writeString(packet, "minecraft:stalagmite");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto stalagmiteData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															 {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "stalagmite");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), stalagmiteData.begin(), stalagmiteData.end());

	// 24. minecraft:outside_border
	writeString(packet, "minecraft:outside_border");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto outsideBorderData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
																{
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "outside_border");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), outsideBorderData.begin(), outsideBorderData.end());

	// 25. minecraft:generic_kill
	writeString(packet, "minecraft:generic_kill");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto genericKillData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
															  {
		Courage::World::NBT::writeTAG_String(nbt, "message_id", "generic_kill");
		Courage::World::NBT::writeTAG_String(nbt, "scaling", "never");
		Courage::World::NBT::writeTAG_Float(nbt, "exhaustion", 0.0f); });
	packet.insert(packet.end(), genericKillData.begin(), genericKillData.end());

	sendRawPacketBin(sock, packet);
}