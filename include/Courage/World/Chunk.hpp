#pragma once
#include <Courage/World/Block.hpp>
#include <Courage/World/Terrain.hpp>
#include <Courage/World/NBT.hpp>

#include <vector>
#include <string>
#include <chrono>
#include <iostream>

namespace Courage::World
{
	struct ChunkComponents
	{
		std::vector<uint8_t> heightmaps;
		std::vector<uint8_t> sections;
		std::vector<uint8_t> blockEntities;
	};

	class Chunk
	{
	public:
		static std::vector<int64_t> packHeightmap(const std::vector<int> &heights);

		static bool isValidBlockIndex(size_t blockIndex, const BlockRegistry &blockRegistry);
		static size_t getSafeBlockIndex(size_t blockIndex, const BlockRegistry &blockRegistry);
		static bool isValidMinecraftBlockName(const std::string &blockName);

		static std::vector<uint8_t> buildChunkNBT(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry);
		static std::vector<uint8_t> buildHeightmapsNBT(int cx, int cz, TerrainGenerator &generator);
		static std::vector<uint8_t> buildSectionsNBT(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry);
		static std::vector<uint8_t> buildSectionsNetwork(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry);
		static ChunkComponents buildChunkComponents(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry);
		static std::vector<int64_t> packBlockStates(const std::vector<uint32_t> &states, int bitsPerBlock);
		
		static void writeNBTCompound(std::vector<uint8_t> &data, const std::string &name);
		static void writeNBTList(std::vector<uint8_t> &data, const std::string &name, uint8_t elementType, int32_t elementCount);
		static void writeNBTByte(std::vector<uint8_t> &data, const std::string &name, uint8_t value);
		static void writeNBTInt(std::vector<uint8_t> &data, const std::string &name, int32_t value);
		static void writeNBTLong(std::vector<uint8_t> &data, const std::string &name, int64_t value);
		static void writeNBTString(std::vector<uint8_t> &data, const std::string &name, const std::string &value);
		static void writeNBTEnd(std::vector<uint8_t> &data);
	};
}