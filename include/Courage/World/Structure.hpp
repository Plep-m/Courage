#pragma once
#include <Courage/World/Noise.hpp>
#include <Courage/World/Biome.hpp>

#include <vector>
#include <cmath>
#include <cstdint>
#include <tuple>

namespace Courage::World
{
	struct Structure
	{
		enum Type
		{
			VILLAGE,
			DUNGEON,
			TREE,
			ORE_VEIN,
			CAVE_SYSTEM
		} type;
		int x, y, z, size;
		std::vector<std::tuple<int, int, int, size_t>> blocks;
	};

	class StructureGenerator
	{
	private:
		NoiseGenerator structNoise;
		std::vector<size_t> woodBlocks, leafBlocks, stoneBlocks;

	public:
		StructureGenerator(uint64_t seed, const std::vector<size_t> &wood, const std::vector<size_t> &leaves, const std::vector<size_t> &stones);
		std::vector<Structure> generateStructuresInChunk(int cx, int cz, BiomeType biome);
	};
}