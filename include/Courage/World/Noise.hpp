#pragma once
#include <Courage/World/FastCache.hpp>
#include <array>
#include <random>

namespace Courage::World
{

	class NoiseGenerator
	{
	private:
		FastCache<float, 8192> heightCache;
		FastCache<float, 4096> detailCache;
		std::array<float, 256> permTable;

		static constexpr std::array<std::array<float, 2>, 8> gradients = {{{{1, 1}}, {{-1, 1}}, {{1, -1}}, {{-1, -1}}, {{1, 0}}, {{-1, 0}}, {{0, 1}}, {{0, -1}}}};

	public:
		NoiseGenerator(uint64_t seed);

		float noise2D(float x, float z);

		float heightNoise(float x, float z);
		float detailNoise(float x, float z, float scale = 0.02f);
	};
}