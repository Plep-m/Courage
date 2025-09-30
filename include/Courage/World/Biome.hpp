#pragma once
#include <string>
#include <vector>

namespace Courage::World
{
	enum class BiomeType : uint8_t
	{
		PLAINS,
		FOREST,
		DESERT,
		MOUNTAINS,
		SWAMP,
		TAIGA,
		OCEAN,
		RIVER
	};

	struct Biome
	{
		BiomeType type;
		std::string name;
		std::string grassColor;
		std::vector<size_t> commonBlocks;
		std::vector<size_t> rareBlocks;
		int minHeight, maxHeight;
		float treeChance;
		float humidity;

		Biome(BiomeType t, const std::string &n, int minH, int maxH, float treeC, float hum = 0.0f)
			: type(t), name(n), minHeight(minH), maxHeight(maxH), treeChance(treeC), humidity(hum) {}
	};
}