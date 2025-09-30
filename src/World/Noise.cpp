#include <Courage/World/Noise.hpp>

namespace Courage::World
{
	NoiseGenerator::NoiseGenerator(uint64_t seed)
	{
		std::mt19937 rng(seed);
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
		for (auto &p : permTable)
			p = dist(rng);
	}

	float NoiseGenerator::noise2D(float x, float z)
	{
		int ix = static_cast<int>(std::floor(x)) & 255;
		int iz = static_cast<int>(std::floor(z)) & 255;

		float fx = x - std::floor(x);
		float fz = z - std::floor(z);

		float u = fx * fx * (3.0f - 2.0f * fx);
		float v = fz * fz * (3.0f - 2.0f * fz);

		int a = (ix + iz * 57) & 255;
		int b = (ix + 1 + iz * 57) & 255;
		int c = (ix + (iz + 1) * 57) & 255;
		int d = (ix + 1 + (iz + 1) * 57) & 255;

		float v1 = permTable[a];
		float v2 = permTable[b];
		float v3 = permTable[c];
		float v4 = permTable[d];

		float i1 = v1 + u * (v2 - v1);
		float i2 = v3 + u * (v4 - v3);

		return i1 + v * (i2 - i1);
	}

	float NoiseGenerator::heightNoise(float x, float z)
	{
		uint64_t key = (static_cast<uint64_t>(x * 16) << 32) | static_cast<uint64_t>(z * 16);
		float result;
		if (heightCache.get(key, result))
			return result;

		float value = 0.0f;
		float amp = 1.0f;
		float freq = 0.005f;

		for (int i = 0; i < 6; i++)
		{
			value += noise2D(x * freq, z * freq) * amp;
			amp *= 0.6f;
			freq *= 2.0f;
		}

		heightCache.put(key, value);
		return value;
	}

	float NoiseGenerator::detailNoise(float x, float z, float scale)
	{
		uint64_t key = (static_cast<uint64_t>(x * scale * 1000) << 32) | static_cast<uint64_t>(z * scale * 1000);
		float result;
		if (detailCache.get(key, result))
			return result;

		result = noise2D(x * scale, z * scale);
		detailCache.put(key, result);
		return result;
	}
}