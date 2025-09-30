#pragma once
#include <Courage/World/FastCache.hpp>
#include <Courage/World/Noise.hpp>
#include <Courage/World/Biome.hpp>

#include <unordered_map>

namespace Courage::World
{
	class BiomeGenerator
	{
	private:
		NoiseGenerator tempNoise, humidNoise;
		std::unordered_map<BiomeType, Biome> biomes;
		FastCache<BiomeType, 4096> biomeCache;

	public:
		BiomeGenerator(uint64_t seed);
		BiomeType getBiomeAt(int x, int z);
		const Biome &getBiome(BiomeType type) const;
	};
}