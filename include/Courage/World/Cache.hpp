#pragma once
#include <unordered_map>
#include <list>
#include <cstddef>

template <typename K, typename V>
class Cache
{
private:
	size_t capacity;

	std::list<std::pair<K, V>> items;
	std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;

public:
	Cache(size_t cap) : capacity(cap) {}

	bool get(const K &key, V &value)
	{
		auto it = map.find(key);
		if (it == map.end())
			return false;

		items.splice(items.begin(), items, it->second);
		value = it->second->second;
		return true;
	}

	void put(const K &key, const V &value)
	{
		auto it = map.find(key);

		if (it != map.end())
		{
			it->second->second = value;
			items.splice(items.begin(), items, it->second);
			return;
		}

		if (items.size() >= capacity)
		{
			auto last = items.back();
			map.erase(last.first);
			items.pop_back();
		}

		// Insertion en tête
		items.emplace_front(key, value);
		map[key] = items.begin();
	}

	bool contains(const K &key) const
	{
		return map.find(key) != map.end();
	}

	size_t size() const
	{
		return items.size();
	}

	size_t max_size() const
	{
		return capacity;
	}
};
