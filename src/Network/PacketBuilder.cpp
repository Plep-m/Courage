#include "Courage/Network/PacketBuilder.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace Courage::Network
{

	std::string PacketBuilder::longToHex(uint64_t value)
	{
		std::ostringstream ss;
		ss << std::hex << std::setfill('0');

		for (int i = 7; i >= 0; --i)
		{
			uint8_t byte = (value >> (i * 8)) & 0xFF;
			ss << std::setw(2) << static_cast<int>(byte);
		}
		return ss.str();
	}
	std::string PacketBuilder::stringToHex(const std::string &str)
	{
		std::stringstream ss;
		for (unsigned char c : str)
			ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;

		return ss.str();
	}

	std::string PacketBuilder::varIntToHex(int value)
	{
		std::stringstream ss;
		do
		{
			uint8_t temp = value & 0x7F;
			value >>= 7;
			if (value != 0)
				temp |= 0x80;

			ss << std::hex << std::setw(2) << std::setfill('0') << (int)temp;
		} while (value != 0);
		return ss.str();
	}

	std::string PacketBuilder::stringToHexWithLength(const std::string &str)
	{
		std::string lengthHex = varIntToHex(str.length());
		std::string contentHex = stringToHex(str);

		return lengthHex + contentHex;
	}

	std::string PacketBuilder::buildBrandPacket(const std::string &brandName)
	{
		std::string packet = "01";
		packet += stringToHexWithLength("minecraft:brand");
		packet += stringToHexWithLength(brandName);

		return packet;
	}

	std::string PacketBuilder::doubleToHex(double value)
	{
		uint64_t bits;
		std::memcpy(&bits, &value, sizeof(double));

		std::stringstream ss;
		for (int i = 7; i >= 0; i--)
		{
			uint8_t byte = (bits >> (8 * i)) & 0xFF;
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
		}
		return ss.str();
	}

	std::string PacketBuilder::buildFeatureFlags(const std::string &namespaceName)
	{
		std::string packet = "0c";
		packet += "01";
		packet += stringToHexWithLength(namespaceName);

		return packet;
	}

	std::string PacketBuilder::buildKnownPacks(const std::string &namespaceName, const std::string &packName, const std::string &version)
	{

		std::string packet = varIntToHex(0x0E);
		packet += varIntToHex(1);
		packet += stringToHexWithLength(namespaceName);
		packet += stringToHexWithLength(packName);
		packet += stringToHexWithLength(version);

		return packet;
	}

	std::string PacketBuilder::uuidToHex(const std::array<uint8_t, 16> &uuidBytes)
	{
		std::stringstream ss;
		for (uint8_t byte : uuidBytes)
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
		return ss.str();
	}

	std::string PacketBuilder::floatToHex(float value)
	{
		uint32_t bits;
		std::memcpy(&bits, &value, sizeof(float));

		std::stringstream ss;
		for (int i = 3; i >= 0; --i)
		{
			uint8_t byte = (bits >> (8 * i)) & 0xFF;
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
		}
		return ss.str();
	}

	std::string PacketBuilder::buildWorldBorderPacket(
		double centerX,
		double centerZ,
		double oldSize,
		double newSize,
		int64_t speed,
		int32_t portalTeleportBoundary,
		int32_t warningBlocks,
		int32_t warningTime)
	{
		std::string packet = "25";

		packet += doubleToHex(centerX);
		packet += doubleToHex(centerZ);

		packet += doubleToHex(oldSize);
		packet += doubleToHex(newSize);

		packet += varIntToHex(static_cast<int>(speed));
		packet += varIntToHex(portalTeleportBoundary);
		packet += varIntToHex(warningBlocks);
		packet += varIntToHex(warningTime);

		return packet;
	}

	std::string PacketBuilder::buildPlayerPositionPacket(double x, double y, double z, float yaw, float pitch, uint8_t flags, int32_t teleportId)
	{
		std::stringstream packet;

		packet << varIntToHex(0x40);
		packet << doubleToHex(x);
		packet << doubleToHex(y);
		packet << doubleToHex(z);
		packet << floatToHex(yaw);
		packet << floatToHex(pitch);
		packet << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(flags);
		packet << varIntToHex(teleportId);

		return packet.str();
	}

	std::string PacketBuilder::buildDifficultyPacket(uint8_t difficulty, bool locked)
	{
		std::stringstream packet;

		packet << varIntToHex(0x0B);
		packet << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(difficulty);
		packet << (locked ? "01" : "00");

		return packet.str();
	}

	std::string PacketBuilder::buildPlayerAbilitiesPacket(uint8_t flags, float flyingSpeed, float fieldOfViewModifier)
	{
		std::stringstream packet;

		packet << varIntToHex(0x38);
		packet << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(flags);
		packet << floatToHex(flyingSpeed);
		packet << floatToHex(fieldOfViewModifier);

		return packet.str();
	}

	std::string PacketBuilder::buildHeldItemSlotPacket(int8_t slot)
	{
		std::stringstream packet;
		packet << varIntToHex(0x53);
		packet << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(slot);

		return packet.str();
	}

	std::string PacketBuilder::buildTimeUpdatePacket(int64_t worldAge, int64_t dayTime)
	{
		std::string packet = "64";

		auto longToHex = [](int64_t value) -> std::string
		{
			std::stringstream ss;
			for (int i = 7; i >= 0; --i)
			{
				uint8_t byte = (value >> (8 * i)) & 0xFF;
				ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
			}
			return ss.str();
		};
		packet += longToHex(worldAge);
		packet += longToHex(dayTime);

		return packet;
	}

	std::string PacketBuilder::buildSpawnPosition(int32_t x, int32_t y, int32_t z)
	{

		auto intToHex = [](int32_t value) -> std::string
		{
			std::stringstream ss;
			for (int i = 3; i >= 0; --i)
			{
				uint8_t byte = (value >> (8 * i)) & 0xFF;
				ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
			}
			return ss.str();
		};
		std::string packet = "56";
		packet += intToHex(x);
		packet += intToHex(y);
		packet += intToHex(z);

		return packet;
	}

	std::string PacketBuilder::buildGameStateChangePacket(uint8_t reason, float value)
	{
		std::stringstream packet;

		packet << varIntToHex(0x22);
		packet << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(reason);
		packet << floatToHex(value);

		return packet.str();
	}

	std::string PacketBuilder::buildStepTickPacket(int32_t steps)
	{
		std::stringstream packet;
		packet << varIntToHex(0x72);
		packet << varIntToHex(steps);

		return packet.str();
	}

	std::string PacketBuilder::buildUpdateViewPositionPacket(int32_t chunkX, int32_t chunkZ)
	{
		std::stringstream packet;
		packet << varIntToHex(0x54);
		packet << varIntToHex(chunkX);
		packet << varIntToHex(chunkZ);

		return packet.str();
	}

	std::string PacketBuilder::buildExperiencePacket(float experienceBar, int32_t level, int32_t totalExperience)
	{
		std::stringstream packet;
		packet << varIntToHex(0x5C);
		packet << floatToHex(experienceBar);
		packet << varIntToHex(level);
		packet << varIntToHex(totalExperience);

		return packet.str();
	}
}
