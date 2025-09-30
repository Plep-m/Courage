#include <Courage/World/BiomeGenerator.hpp>

namespace Courage::World
{
	BiomeGenerator::BiomeGenerator(uint64_t seed) : tempNoise(seed + 1000), humidNoise(seed + 2000)
	{
		biomes.emplace(BiomeType::PLAINS, Biome(BiomeType::PLAINS, "minecraft:plains", 60, 80, 0.05f, 0.04f));
		biomes.emplace(BiomeType::FOREST, Biome(BiomeType::FOREST, "minecraft:forest", 65, 85, 0.3f, 0.04f));
		biomes.emplace(BiomeType::DESERT, Biome(BiomeType::DESERT, "minecraft:desert", 55, 75, 0.01f, 0.04f));
		biomes.emplace(BiomeType::MOUNTAINS, Biome(BiomeType::MOUNTAINS, "minecraft:windswept_hills", 70, 120, 0.1f, 0.04f));
		biomes.emplace(BiomeType::SWAMP, Biome(BiomeType::SWAMP, "minecraft:swamp", 58, 65, 0.4f, 0.04f));
		biomes.emplace(BiomeType::TAIGA, Biome(BiomeType::TAIGA, "minecraft:taiga", 62, 85, 0.6f, 0.04f));
		biomes.emplace(BiomeType::OCEAN, Biome(BiomeType::OCEAN, "minecraft:ocean", 45, 55, 0.0f, 0.04f));
	}

	BiomeType BiomeGenerator::getBiomeAt(int x, int z)
	{
		uint64_t key = (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(z);
		BiomeType result;
		if (biomeCache.get(key, result))
			return result;

		float temp = tempNoise.detailNoise(x, z, 0.003f);
		float humid = humidNoise.detailNoise(x, z, 0.004f);

		if (temp > 0.6f && humid < -0.3f)
			result = BiomeType::DESERT;
		else if (temp < -0.4f)
			result = BiomeType::TAIGA;
		else if (humid > 0.4f && temp < 0.2f)
			result = BiomeType::SWAMP;
		else if (temp > 0.3f && humid > 0.0f)
			result = BiomeType::FOREST;
		else if (temp > -0.2f && humid > -0.3f)
			result = BiomeType::PLAINS;
		else
			result = BiomeType::MOUNTAINS;

		biomeCache.put(key, result);
		return result;
	}

	const Biome &BiomeGenerator::getBiome(BiomeType type) const
	{
		return biomes.at(type);
	}
}