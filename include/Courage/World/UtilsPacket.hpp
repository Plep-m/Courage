#pragma once
#include <vector>
#include <cstdint>
#include <string>

void writeVarLong(std::vector<uint8_t> &buffer, int64_t value);
void writeVarInt(std::vector<uint8_t> &buffer, int32_t value);
void writeInt32(std::vector<uint8_t> &buffer, int32_t value);
void writeByte(std::vector<uint8_t> &buffer, uint8_t value);
void writeBool(std::vector<uint8_t> &buffer, bool value);
void writeLongArray(std::vector<uint8_t> &buffer, const std::vector<int64_t> &array);
void writeByteArray(std::vector<uint8_t> &buffer, const std::vector<uint8_t> &data);
void writeInt64(std::vector<uint8_t> &buffer, int64_t value);
void writeInt16(std::vector<uint8_t> &buffer, uint16_t value);
void writeString(std::vector<uint8_t> &buffer, const std::string &str);
void writeLong(std::vector<uint8_t> &buffer, int64_t value);
void writeBytes(std::vector<uint8_t> &out, const std::vector<uint8_t> &src);
void writeShort(std::vector<uint8_t> &out, int16_t value);
void writeStringRaw(std::vector<uint8_t> &out, const std::string &str);
void sendRawPacketBin(int sock, const std::vector<uint8_t> &data);
void sendRawPacket(int socket, const std::string &hex);