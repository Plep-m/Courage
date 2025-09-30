#pragma once
#include <cmath>
#include <string>
#include <array>
#include <cstdint>

namespace Courage::World
{
	template <typename T, size_t Size>
	class FastCache
	{
	private:
		struct CacheEntry
		{
			uint64_t key;
			T value;
			bool valid;
			CacheEntry(uint64_t k = 0, const T &v = T(), bool val = false) : key(k), value(v), valid(val) {}
		};
		std::array<CacheEntry, Size> cache;

	public:
		bool get(uint64_t key, T &out)
		{
			size_t idx = key % Size;
			if (cache[idx].valid && cache[idx].key == key)
			{
				out = cache[idx].value;
				return true;
			}
			return false;
		}

		void put(uint64_t key, const T &value)
		{
			size_t idx = key % Size;
			cache[idx] = {key, value, true};
		}
	};
}