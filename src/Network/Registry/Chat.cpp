#include "Courage/Network/Registry.hpp"

void Courage::Network::Registry::sendChatTypeRegistry(int sock)
{
	std::vector<uint8_t> packet;

	writeVarInt(packet, REGISTRY_DATA_PACKET_ID);
	writeString(packet, "minecraft:chat_type");
	writeVarInt(packet, 7);

	// 1. minecraft:chat
	writeString(packet, "minecraft:chat");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto chatData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													   {
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "chat.type.text");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), chatData.begin(), chatData.end());

	// 2. minecraft:emote_command
	writeString(packet, "minecraft:emote_command");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto emoteData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "chat.type.emote");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.emote");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), emoteData.begin(), emoteData.end());

	// 3. minecraft:msg_command_incoming
	writeString(packet, "minecraft:msg_command_incoming");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto msgInData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														{
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "commands.message.display.incoming");
            auto styleCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &style) {
                Courage::World::NBT::writeTAG_String(style, "color", "gray");
                Courage::World::NBT::writeTAG_Byte(style, "italic", 1);
            });
            Courage::World::NBT::writeTAG_Compound(chat, "style", styleCompound);
            
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), msgInData.begin(), msgInData.end());

	// 4. minecraft:msg_command_outgoing
	writeString(packet, "minecraft:msg_command_outgoing");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto msgOutData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "commands.message.display.outgoing");
            
            auto styleCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &style) {
                Courage::World::NBT::writeTAG_String(style, "color", "gray");
                Courage::World::NBT::writeTAG_Byte(style, "italic", 1);
            });
            Courage::World::NBT::writeTAG_Compound(chat, "style", styleCompound);
            
            std::vector<std::string> params = {"target", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), msgOutData.begin(), msgOutData.end());

	// 5. minecraft:say_command
	writeString(packet, "minecraft:say_command");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto sayData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
													  {
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "chat.type.announcement");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), sayData.begin(), sayData.end());

	// 6. minecraft:team_msg_command_incoming
	writeString(packet, "minecraft:team_msg_command_incoming");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto teamInData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														 {
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "chat.type.team.text");
            std::vector<std::string> params = {"target", "sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), teamInData.begin(), teamInData.end());

	// 7. minecraft:team_msg_command_outgoing
	writeString(packet, "minecraft:team_msg_command_outgoing");
	packet.push_back(0x01);
	packet.push_back(0x0A);
	auto teamOutData = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &nbt)
														  {
        auto chatCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &chat) {
            Courage::World::NBT::writeTAG_String(chat, "translation_key", "chat.type.team.sent");
            std::vector<std::string> params = {"target", "sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(chat, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "chat", chatCompound);
        
        auto narrationCompound = Courage::World::NBT::buildCompound([](std::vector<uint8_t> &narration) {
            Courage::World::NBT::writeTAG_String(narration, "translation_key", "chat.type.text.narrate");
            std::vector<std::string> params = {"sender", "content"};
            Courage::World::NBT::writeTAG_List_of_String(narration, "parameters", params);
        });
        Courage::World::NBT::writeTAG_Compound(nbt, "narration", narrationCompound); });
	packet.insert(packet.end(), teamOutData.begin(), teamOutData.end());

	sendRawPacketBin(sock, packet);
}