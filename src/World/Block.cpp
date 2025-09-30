#include <Courage/World/Block.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Courage::World
{
	BlockRegistry::BlockRegistry()
	{
		nextStateId = 0;

		loadFromJson("../blocks.json");
	}

	void BlockRegistry::loadFromJson(const std::string &path)
	{
		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cerr << "Impossible d'ouvrir " << path << std::endl;
			return;
		}

		json j;
		try
		{
			f >> j;
		}
		catch (const std::exception &e)
		{
			std::cerr << "Erreur JSON: " << e.what() << std::endl;
			return;
		}

		if (!j.is_array())
		{
			std::cerr << "ERREUR: Le JSON attendu doit être un tableau (blocks.json de PrismarineJS)" << std::endl;
			return;
		}

		for (const auto &block : j)
		{
			if (!block.contains("name"))
				continue;

			std::string name = "minecraft:" + block.value("name", "unknown");

			int minId = block.value("minStateId", -1);
			int maxId = block.value("maxStateId", -1);
			int defaultId = block.value("defaultState", minId);

			addBlock(name);
			size_t blockIndex = nameToIndex[name];

			if (defaultId >= 0)
				blockIndexToDefaultStateId[blockIndex] = defaultId;

			std::vector<std::pair<std::string, std::vector<std::string>>> props;
			if (block.contains("states") && block["states"].is_array())
			{
				for (auto &prop : block["states"])
				{
					std::string propName = prop.value("name", "");
					std::string type = prop.value("type", "");
					int numValues = prop.value("num_values", 0);

					std::vector<std::string> values;
					if (type == "bool")
						values = {"false", "true"};
					else if (type == "int")
					{
						for (int v = 0; v < numValues; v++)
							values.push_back(std::to_string(v));
					}
					else if (type == "enum" && prop.contains("values"))
					{
						for (auto &val : prop["values"])
							values.push_back(val.get<std::string>());
					}

					if (!propName.empty() && !values.empty())
						props.push_back({propName, values});
				}
			}

			int idCounter = minId;

			std::function<void(size_t, std::unordered_map<std::string, std::string>)> backtrack;
			backtrack = [&](size_t index, std::unordered_map<std::string, std::string> current)
			{
				if (index == props.size())
				{
					if (idCounter <= maxId)
					{
						int stateId = idCounter++;
						stateIdToBlockIndex[stateId] = blockIndex;
						stateIdToProperties[stateId] = current;

						if (stateId == defaultId)
							blockIndexToStateId[blockIndex] = stateId;

						nextStateId = std::max(nextStateId, stateId + 1);
					}
					return;
				}
				const auto &[key, values] = props[index];
				for (const auto &val : values)
				{
					auto next = current;
					next[key] = val;
					backtrack(index + 1, next);
				}
			};

			if (!props.empty())
				backtrack(0, {});
			else
			{
				int stateId = defaultId;
				stateIdToBlockIndex[stateId] = blockIndex;
				stateIdToProperties[stateId] = {};
				blockIndexToStateId[blockIndex] = stateId;
				nextStateId = std::max(nextStateId, stateId + 1);
			}
		}
	}

	void BlockRegistry::addBlock(const std::string &name, const std::unordered_map<std::string, std::string> &properties)
	{
		size_t blockIndex = blocks.size();
		nameToIndex[name] = blockIndex;

		BlockData block{name, properties};
		blocks.push_back(block);
	}

	size_t BlockRegistry::getIndex(const std::string &name) const
	{
		auto it = nameToIndex.find(name);
		if (it == nameToIndex.end())
		{
			std::cerr << "WARNING: Block not found: " << name << ", using air instead\n";
			return 0;
		}
		return it->second;
	}

	const BlockRegistry::BlockData &BlockRegistry::getBlock(size_t index) const
	{
		if (index >= blocks.size())
			return blocks[0];
		return blocks[index];
	}

	int32_t BlockRegistry::getDefaultStateId(size_t blockIndex) const
	{
		auto it = blockIndexToDefaultStateId.find(blockIndex);
		if (it != blockIndexToDefaultStateId.end())
			return it->second;
		return 0;
	}

	int32_t BlockRegistry::getStateId(size_t blockIndex) const
	{
		auto it = blockIndexToStateId.find(blockIndex);
		if (it != blockIndexToStateId.end())
			return it->second;
		return 0;
	}

	std::vector<BlockRegistry::BlockStateEntry> BlockRegistry::getStatesForBlock(size_t blockIndex) const
	{
		std::vector<BlockStateEntry> result;
		for (const auto &[stateId, idx] : stateIdToBlockIndex)
		{
			if (idx == blockIndex)
				result.push_back({stateIdToProperties.at(stateId), stateId});
		}
		std::sort(result.begin(), result.end(), [](auto &a, auto &b)
				  { return a.stateId < b.stateId; });
		return result;
	}

	size_t BlockRegistry::getBlockIndexFromStateId(int32_t stateId) const
	{
		auto it = stateIdToBlockIndex.find(stateId);
		if (it != stateIdToBlockIndex.end())
			return it->second;
		return 0;
	}

	size_t BlockRegistry::count() const { return blocks.size(); }
	int32_t BlockRegistry::getTotalStateCount() const { return nextStateId; }

	std::vector<std::string> BlockRegistry::splitValues(const std::string &valueList)
	{
		std::vector<std::string> result;
		std::stringstream ss(valueList);
		std::string item;
		while (std::getline(ss, item, ','))
			result.push_back(item);
		return result;
	}

	std::vector<std::unordered_map<std::string, std::string>>
	BlockRegistry::getAllPropertyCombinations(const std::unordered_map<std::string, std::string> &properties) const
	{
		std::vector<std::unordered_map<std::string, std::string>> result;

		if (properties.empty())
		{
			result.push_back({});
			return result;
		}

		std::vector<std::pair<std::string, std::vector<std::string>>> props;
		for (const auto &[key, value] : properties)
		{
			auto values = splitValues(value);
			props.push_back({key, values});
		}

		std::function<void(size_t, std::unordered_map<std::string, std::string>)> backtrack;
		backtrack = [&](size_t index, std::unordered_map<std::string, std::string> current)
		{
			if (index == props.size())
			{
				result.push_back(current);
				return;
			}
			const auto &[key, values] = props[index];
			for (const auto &val : values)
			{
				auto next = current;
				next[key] = val;
				backtrack(index + 1, next);
			}
		};

		backtrack(0, {});
		return result;
	}
}
