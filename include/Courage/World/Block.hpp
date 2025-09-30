#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Courage::World
{
	class BlockRegistry
	{
	public:
		struct BlockData
		{
			std::string name;
			std::unordered_map<std::string, std::string> properties;
		};

		struct BlockStateEntry
		{
			std::unordered_map<std::string, std::string> properties;
			int32_t stateId;
		};

		BlockRegistry();

		static BlockRegistry &getInstance()
		{
			static BlockRegistry instance;
			return instance;
		}

		void loadFromJson(const std::string &path);

		void addBlock(const std::string &name, const std::unordered_map<std::string, std::string> &properties = {});
		size_t getIndex(const std::string &name) const;
		const BlockData &getBlock(size_t index) const;

		int32_t getDefaultStateId(size_t blockIndex) const;
		int32_t getStateId(size_t blockIndex) const;
		size_t getBlockIndexFromStateId(int32_t stateId) const;

		std::vector<BlockStateEntry> getStatesForBlock(size_t blockIndex) const;

		size_t count() const;
		int32_t getTotalStateCount() const;

		static std::vector<std::string> splitValues(const std::string &valueList);
		std::vector<std::unordered_map<std::string, std::string>>
		getAllPropertyCombinations(const std::unordered_map<std::string, std::string> &properties) const;

	private:
		std::vector<BlockData> blocks;
		std::unordered_map<std::string, size_t> nameToIndex;
		std::unordered_map<size_t, int32_t> blockIndexToDefaultStateId;
		std::unordered_map<size_t, int32_t> blockIndexToStateId;
		std::unordered_map<int32_t, size_t> stateIdToBlockIndex;
		std::unordered_map<int32_t, std::unordered_map<std::string, std::string>> stateIdToProperties;

		int32_t nextStateId;
	};
}
