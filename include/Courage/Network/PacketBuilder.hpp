#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace Courage::Network
{
	class PacketBuilder
	{
	public:
		struct GameStateReason
		{
			static constexpr uint8_t INVALID_BED = 0;
			static constexpr uint8_t BEGIN_RAINING = 1;
			static constexpr uint8_t END_RAINING = 2;
			static constexpr uint8_t CHANGE_GAMEMODE = 3;
			static constexpr uint8_t ENTER_CREDITS = 4;
			static constexpr uint8_t DEMO_MESSAGE = 5;
			static constexpr uint8_t ARROW_HIT_PLAYER = 6;
			static constexpr uint8_t FADE_VALUE = 7;
			static constexpr uint8_t FADE_TIME = 8;
			static constexpr uint8_t PLAY_PUFFERFISH_STING_SOUND = 9;
			static constexpr uint8_t PLAY_ELDER_GUARDIAN_APPEARANCE = 10;
			static constexpr uint8_t ENABLE_RESPAWN_SCREEN = 11;
			static constexpr uint8_t NONE = 13;
		};

		static std::string buildBrandPacket(const std::string &brandName);
		static std::string buildFeatureFlags(const std::string &namespaceName = "minecraft:courage");
		static std::string buildKnownPacks(const std::string &namespaceName, const std::string &packName, const std::string &version);

		static std::string longToHex(uint64_t value);
		static std::string stringToHex(const std::string &str);
		static std::string varIntToHex(int value);
		static std::string doubleToHex(double value);
		static std::string floatToHex(float value);
		static std::string stringToHexWithLength(const std::string &str);
		static std::string uuidToHex(const std::array<uint8_t, 16> &uuidBytes);
		static std::string buildWorldBorderPacket(double centerX = 0.0, double centerZ = 0.0, double oldSize = 0.0, double newSize = 59999968.0,
												  int64_t speed = 0, int32_t portalTeleportBoundary = 29999984, int32_t warningBlocks = 5, int32_t warningTime = 15);
		static std::string buildPlayerPositionPacket(double x, double y, double z, float yaw, float pitch, uint8_t flags, int32_t teleportId);
		static std::string buildDifficultyPacket(uint8_t difficulty, bool locked);
		static std::string buildPlayerAbilitiesPacket(uint8_t flags, float flyingSpeed, float fieldOfViewModifier);
		static std::string buildHeldItemSlotPacket(int8_t slot);
		static std::string buildTimeUpdatePacket(int64_t worldAge, int64_t dayTime);
		static std::string buildSpawnPosition(int32_t x, int32_t y, int32_t z);
		static std::string buildGameStateChangePacket(uint8_t reason, float value);
		static std::string buildStepTickPacket(int32_t steps);
		static std::string buildUpdateViewPositionPacket(int32_t chunkX, int32_t chunkZ);
		static std::string buildExperiencePacket(float experienceBar, int32_t level, int32_t totalExperience);

	private:
		PacketBuilder() = delete;
	};

}
