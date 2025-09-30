#pragma once

#include "./Noise.hpp"
#include "./BiomeGenerator.hpp"
#include "./Structure.hpp"
#include "./Block.hpp"
#include "./Cache.hpp"

#include <iostream>

namespace Courage::World
{
	struct OreConfig
	{
		size_t blockIndex;
		int maxDepth;
		float threshold;
	};

	class TerrainGenerator
	{
	private:
		NoiseGenerator heightNoise, caveNoise, oreNoise;
		StructureGenerator structGen;
		BlockRegistry &blockRegistry;

		Cache<uint64_t, int> heightCache{1024};
		Cache<uint64_t, size_t> blockCache{4096};

		std::array<OreConfig, 8> oreLUT;

		size_t airIndex;
		size_t stoneIndex;
		size_t dirtIndex;
		size_t grassIndex;
		size_t bedrockIndex;
		size_t sandIndex;
		size_t graniteIndex;
		size_t dioriteIndex;
		size_t andesiteIndex;

		void validateBlockIndices();

	public:
		TerrainGenerator(uint64_t seed, BlockRegistry &registry);

		int getHeightAt(int x, int z);
		size_t getBlockAt(int x, int y, int z);
		std::vector<Structure> getStructuresInChunk(int cx, int cz);
		BiomeType getBiomeForChunk(int cx, int cz);

		void debugBlockIndices() const;

		static constexpr int WORLD_HEIGHT_MIN = -64;
		static constexpr int WORLD_HEIGHT_MAX = 320;
		static constexpr int SECTION_HEIGHT = 16;
		static constexpr int CHUNK_SIZE = 16;
		BiomeGenerator biomeGen;
	};
}