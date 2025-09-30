#include "Courage/World/Terrain.hpp"

namespace Courage::World
{
	TerrainGenerator::TerrainGenerator(uint64_t seed, BlockRegistry &registry)
		: heightNoise(seed),
		  caveNoise(seed + 1),
		  oreNoise(seed + 2),
		  biomeGen(seed),
		  structGen(seed,
					std::vector<size_t>{
						registry.getIndex("minecraft:oak_log"),
						registry.getIndex("minecraft:spruce_log"),
						registry.getIndex("minecraft:birch_log")},
					std::vector<size_t>{
						registry.getIndex("minecraft:oak_leaves"),
						registry.getIndex("minecraft:spruce_leaves"),
						registry.getIndex("minecraft:birch_leaves")},
					std::vector<size_t>{
						registry.getIndex("minecraft:stone"),
						registry.getIndex("minecraft:cobblestone")}),
		  blockRegistry(registry)
	{
		airIndex = blockRegistry.getIndex("minecraft:air");
		stoneIndex = blockRegistry.getIndex("minecraft:stone");
		dirtIndex = blockRegistry.getIndex("minecraft:dirt");
		grassIndex = blockRegistry.getIndex("minecraft:grass_block");
		bedrockIndex = blockRegistry.getIndex("minecraft:bedrock");
		sandIndex = blockRegistry.getIndex("minecraft:sand");
		graniteIndex = blockRegistry.getIndex("minecraft:granite");
		dioriteIndex = blockRegistry.getIndex("minecraft:diorite");
		andesiteIndex = blockRegistry.getIndex("minecraft:andesite");

		oreLUT[0] = {blockRegistry.getIndex("minecraft:coal_ore"), 128, 0.7f};
		oreLUT[1] = {blockRegistry.getIndex("minecraft:iron_ore"), 64, 0.75f};
		oreLUT[2] = {blockRegistry.getIndex("minecraft:copper_ore"), 64, 0.76f};
		oreLUT[3] = {blockRegistry.getIndex("minecraft:gold_ore"), 32, 0.8f};
		oreLUT[4] = {blockRegistry.getIndex("minecraft:lapis_ore"), 32, 0.82f};
		oreLUT[5] = {blockRegistry.getIndex("minecraft:redstone_ore"), 32, 0.78f};
		oreLUT[6] = {blockRegistry.getIndex("minecraft:diamond_ore"), 16, 0.85f};
		oreLUT[7] = {blockRegistry.getIndex("minecraft:emerald_ore"), 32, 0.87f};

		validateBlockIndices();
	}

	void TerrainGenerator::validateBlockIndices()
	{
		size_t maxIndex = blockRegistry.count() - 1;

		if (airIndex > maxIndex || stoneIndex > maxIndex || dirtIndex > maxIndex ||
			grassIndex > maxIndex || bedrockIndex > maxIndex || sandIndex > maxIndex)
		{
			throw std::runtime_error("Invalid basic block indices in TerrainGenerator");
		}

		for (const auto &ore : oreLUT)
		{
			if (ore.blockIndex > maxIndex)
				throw std::runtime_error("Invalid ore block index in TerrainGenerator");
		}
	}

	int TerrainGenerator::getHeightAt(int x, int z)
	{
		uint64_t key = (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(z);
		int result;
		if (heightCache.get(key, result))
			return result;

		BiomeType biome = biomeGen.getBiomeAt(x, z);
		const auto &biomeData = biomeGen.getBiome(biome);

		float height = heightNoise.heightNoise(x, z);
		result = biomeData.minHeight + static_cast<int>((height + 1.0f) * 0.5f * (biomeData.maxHeight - biomeData.minHeight));

		heightCache.put(key, result);
		return result;
	}

	size_t TerrainGenerator::getBlockAt(int x, int y, int z)
	{
		uint64_t key = (static_cast<uint64_t>(x & 0xFFF) << 20) |
					   (static_cast<uint64_t>(y & 0x1FF) << 12) |
					   static_cast<uint64_t>(z & 0xFFF);
		size_t result;
		if (blockCache.get(key, result))
			return result;

		int surfaceHeight = getHeightAt(x, z);
		BiomeType biome = biomeGen.getBiomeAt(x, z);

		if (y <= WORLD_HEIGHT_MIN + 5)
		{
			result = (y == WORLD_HEIGHT_MIN ||
					  (y <= WORLD_HEIGHT_MIN + 4 && heightNoise.detailNoise(x, z) < 0.7f))
						 ? bedrockIndex
						 : stoneIndex;
			blockCache.put(key, result);
			return result;
		}

		if (y > WORLD_HEIGHT_MIN + 5 && y < surfaceHeight - 5)
		{
			float cave = caveNoise.detailNoise(x + y * 0.2f, z + y * 0.2f, 0.02f);
			if (cave > 0.6f)
			{
				result = airIndex;
				blockCache.put(key, result);
				return result;
			}
		}

		if (y < surfaceHeight - 5)
		{
			float ore = oreNoise.detailNoise(x, z + y);
			int depth = WORLD_HEIGHT_MAX - y;

			for (const auto &oreEntry : oreLUT)
			{
				if (depth >= oreEntry.maxDepth && ore > oreEntry.threshold)
				{
					result = oreEntry.blockIndex;
					blockCache.put(key, result);
					return result;
				}
			}

			float stoneVar = heightNoise.detailNoise(x * 0.1f, z * 0.1f);
			if (stoneVar > 0.6f)
				result = graniteIndex;
			else if (stoneVar > 0.2f)
				result = dioriteIndex;
			else if (stoneVar > -0.2f)
				result = andesiteIndex;
			else
				result = stoneIndex;
		}
		else if (y < surfaceHeight - 1)
			result = dirtIndex;
		else if (y < surfaceHeight)
			result = (biome == BiomeType::DESERT) ? sandIndex : grassIndex;
		else
			result = airIndex;

		if (result >= blockRegistry.count())
			result = airIndex;

		blockCache.put(key, result);
		return result;
	}

	std::vector<Structure> TerrainGenerator::getStructuresInChunk(int cx, int cz)
	{
		BiomeType biome = biomeGen.getBiomeAt(cx * 16 + 8, cz * 16 + 8);
		return structGen.generateStructuresInChunk(cx, cz, biome);
	}

	BiomeType TerrainGenerator::getBiomeForChunk(int cx, int cz)
	{
		return biomeGen.getBiomeAt(cx * 16 + 8, cz * 16 + 8);
	}
}