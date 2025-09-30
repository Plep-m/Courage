#include <Courage/World/Chunk.hpp>
#include "Courage/World/Terrain.hpp"
#include "Courage/World/UtilsPacket.hpp"

namespace Courage::World
{
	std::vector<uint8_t> Chunk::buildChunkNBT(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry)
	{
		auto start = std::chrono::high_resolution_clock::now();

		std::vector<uint8_t> root;
		root.reserve(65536);

		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be16(root, 0);

		constexpr int MINECRAFT_DATA_VERSION = 3837;
		NBT::writeTAG_Int(root, "DataVersion", MINECRAFT_DATA_VERSION);
		NBT::writeTAG_Int(root, "xPos", cx);
		NBT::writeTAG_Int(root, "zPos", cz);
		NBT::writeTAG_Int(root, "yPos", TerrainGenerator::WORLD_HEIGHT_MIN / TerrainGenerator::SECTION_HEIGHT);
		NBT::writeTAG_String(root, "Status", "minecraft:full");
		NBT::writeTAG_Long(root, "LastUpdate", 0);

		auto structures = generator.getStructuresInChunk(cx, cz);

		std::vector<std::vector<uint8_t>> sectionElements;
		const int minSection = TerrainGenerator::WORLD_HEIGHT_MIN / TerrainGenerator::SECTION_HEIGHT;
		const int maxSection = TerrainGenerator::WORLD_HEIGHT_MAX / TerrainGenerator::SECTION_HEIGHT;

		sectionElements.reserve(maxSection - minSection);

		for (int sY = minSection; sY < maxSection; ++sY)
		{
			auto comp = NBT::buildCompound([sY, cx, cz, &generator, &blockRegistry, &structures](std::vector<uint8_t> &c)
										   {
                c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Byte));
                NBT::writeTagName(c, "Y");
                c.push_back(static_cast<uint8_t>(sY));

                c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
                NBT::writeTagName(c, "block_states");

                std::unordered_map<size_t, uint32_t> blockToPaletteIndex;
                std::vector<size_t> sectionBlocks;
                std::vector<uint32_t> blockIndices;
                blockIndices.reserve(4096);
                
                for (int y = 0; y < TerrainGenerator::SECTION_HEIGHT; ++y) {
                    int worldY = sY * TerrainGenerator::SECTION_HEIGHT + y;
                    for (int z = 0; z < TerrainGenerator::CHUNK_SIZE; ++z) {
                        int worldZ = cz * TerrainGenerator::CHUNK_SIZE + z;
                        for (int x = 0; x < TerrainGenerator::CHUNK_SIZE; ++x) {
                            int worldX = cx * TerrainGenerator::CHUNK_SIZE + x;
                            
                            size_t blockIndex = generator.getBlockAt(worldX, worldY, worldZ);

                            for (const auto& structure : structures) {
                                for (const auto& [sx, sy, sz, structBlockIndex] : structure.blocks) {
                                    if (worldX == structure.x + sx && 
                                        worldY == structure.y + sy && 
                                        worldZ == structure.z + sz &&
                                        worldY >= sY * TerrainGenerator::SECTION_HEIGHT && 
                                        worldY < (sY + 1) * TerrainGenerator::SECTION_HEIGHT) {
                                        blockIndex = getSafeBlockIndex(structBlockIndex, blockRegistry);
                                        break;
                                    }
                                }
                            }

                            blockIndex = getSafeBlockIndex(blockIndex, blockRegistry);
                            
                            auto it = blockToPaletteIndex.find(blockIndex);
                            if (it == blockToPaletteIndex.end()) {
                                blockToPaletteIndex[blockIndex] = static_cast<uint32_t>(sectionBlocks.size());
                                sectionBlocks.push_back(blockIndex);
                            }
                            
                            blockIndices.push_back(blockToPaletteIndex[blockIndex]);
                        }
                    }
                }

                std::vector<std::vector<uint8_t>> palElems;
                palElems.reserve(sectionBlocks.size());
                
                for (size_t blockIndex : sectionBlocks)
                {
                    const auto &block = blockRegistry.getBlock(blockIndex);
                    
                    if (!isValidMinecraftBlockName(block.name))
                    {
                        std::cerr << "ERROR: Invalid block name format: " << block.name  << " (must be minecraft:name_format)" << std::endl;
                        continue;
                    }
                    
                    palElems.push_back(NBT::buildCompound([&block](std::vector<uint8_t> &p)
                    {
                        NBT::writeTAG_String(p, "Name", block.name);
                        
                        if (!block.properties.empty()) {
                            p.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
                            NBT::writeTagName(p, "Properties");

                            for (const auto& [key, value] : block.properties) {
                                if (!key.empty() && !value.empty()) {
                                    NBT::writeTAG_String(p, key, value);
                                }
                            }
                            p.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));
                        }
                    }));
                }
                
                NBT::writeTAG_List_of_Compound(c, "palette", palElems);
                
                if (sectionBlocks.size() > 1) {
                    auto packed = packBlockStates(blockIndices, static_cast<int>(sectionBlocks.size()));
                    NBT::writeTAG_LongArray(c, "data", packed);
                }

                c.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

                c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
                NBT::writeTagName(c, "biomes");
                
                BiomeType chunkBiome = generator.getBiomeForChunk(cx, cz);
                const auto& biome = generator.biomeGen.getBiome(chunkBiome);

                std::string biomeName = biome.name;
                if (biomeName.substr(0, 10) != "minecraft:") {
                    biomeName = "minecraft:" + biomeName;
                }
                
                std::vector<std::string> bp = {biomeName};
                NBT::writeTAG_List_of_String(c, "palette", bp);
                c.push_back(static_cast<uint8_t>(NBTTagType::TAG_End)); });
			sectionElements.push_back(std::move(comp));
		}

		NBT::writeTAG_List_of_Compound(root, "sections", sectionElements);

		std::vector<int> surfaceHeights;
		surfaceHeights.reserve(256);

		for (int z = 0; z < TerrainGenerator::CHUNK_SIZE; ++z)
		{
			for (int x = 0; x < TerrainGenerator::CHUNK_SIZE; ++x)
			{
				int worldX = cx * TerrainGenerator::CHUNK_SIZE + x;
				int worldZ = cz * TerrainGenerator::CHUNK_SIZE + z;
				surfaceHeights.push_back(generator.getHeightAt(worldX, worldZ) - TerrainGenerator::WORLD_HEIGHT_MIN);
			}
		}

		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::writeTagName(root, "Heightmaps");
		auto hm_surface = packHeightmap(surfaceHeights);
		NBT::writeTAG_LongArray(root, "WORLD_SURFACE", hm_surface);
		NBT::writeTAG_LongArray(root, "MOTION_BLOCKING", hm_surface);
		NBT::writeTAG_LongArray(root, "OCEAN_FLOOR", hm_surface);
		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_List));
		NBT::writeTagName(root, "block_entities");
		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be32(root, 0);

		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_List));
		NBT::writeTagName(root, "Entities");
		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be32(root, 0);

		root.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

		return root;
	}

	std::vector<uint8_t> Chunk::buildHeightmapsNBT(int cx, int cz, TerrainGenerator &generator)
	{
		std::vector<int> heights;
		heights.reserve(16 * 16);

		for (int z = 0; z < 16; z++)
		{
			for (int x = 0; x < 16; x++)
			{
				int worldX = cx * 16 + x;
				int worldZ = cz * 16 + z;
				int height = generator.getHeightAt(worldX, worldZ);
				height = std::clamp(height, -64, 319);
				heights.push_back(height);
			}
		}

		auto packed = packHeightmap(heights);

		std::vector<uint8_t> data;
		data.reserve(512);

		data.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be16(data, 0);

		data.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::writeTagName(data, "Heightmaps");

		NBT::writeTAG_LongArray(data, "MOTION_BLOCKING", packed);
		NBT::writeTAG_LongArray(data, "WORLD_SURFACE", packed);

		data.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));
		data.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

		return data;
	}

	std::vector<uint8_t> Chunk::buildSectionsNBT(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry)
	{
		std::vector<uint8_t> sectionsData;
		sectionsData.reserve(32768);

		sectionsData.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be16(sectionsData, 0);

		auto structures = generator.getStructuresInChunk(cx, cz);

		std::vector<std::vector<uint8_t>> sectionElements;
		const int minSection = TerrainGenerator::WORLD_HEIGHT_MIN / TerrainGenerator::SECTION_HEIGHT;
		const int maxSection = TerrainGenerator::WORLD_HEIGHT_MAX / TerrainGenerator::SECTION_HEIGHT;

		sectionElements.reserve(maxSection - minSection);

		for (int sY = minSection; sY < maxSection; ++sY)
		{
			auto comp = NBT::buildCompound([sY, cx, cz, &generator, &blockRegistry, &structures](std::vector<uint8_t> &c)
										   {
											   c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Byte));
											   NBT::writeTagName(c, "Y");
											   c.push_back(static_cast<uint8_t>(sY));

											   c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
											   NBT::writeTagName(c, "block_states");

											   std::unordered_map<int32_t, uint32_t> stateIdToPaletteIndex;
											   std::vector<int32_t> sectionStateIds;
											   std::vector<uint32_t> paletteIndices;
											   paletteIndices.reserve(4096);

											   for (int y = 0; y < TerrainGenerator::SECTION_HEIGHT; ++y)
											   {
												   int worldY = sY * TerrainGenerator::SECTION_HEIGHT + y;
												   for (int z = 0; z < TerrainGenerator::CHUNK_SIZE; ++z)
												   {
													   int worldZ = cz * TerrainGenerator::CHUNK_SIZE + z;
													   for (int x = 0; x < TerrainGenerator::CHUNK_SIZE; ++x)
													   {
														   int worldX = cx * TerrainGenerator::CHUNK_SIZE + x;

														   size_t blockIndex = getSafeBlockIndex(generator.getBlockAt(worldX, worldY, worldZ), blockRegistry);

														   for (const auto &structure : structures)
														   {
															   for (const auto &[sx, sy, sz, structBlockIndex] : structure.blocks)
															   {
																   if (worldX == structure.x + sx &&
																	   worldY == structure.y + sy &&
																	   worldZ == structure.z + sz &&
																	   worldY >= sY * TerrainGenerator::SECTION_HEIGHT &&
																	   worldY < (sY + 1) * TerrainGenerator::SECTION_HEIGHT)
																   {
																	   blockIndex = getSafeBlockIndex(structBlockIndex, blockRegistry);
																	   break;
																   }
															   }
														   }

														   int32_t stateId = blockRegistry.getStateId(blockIndex);

														   auto it = stateIdToPaletteIndex.find(stateId);
														   if (it == stateIdToPaletteIndex.end())
														   {
															   stateIdToPaletteIndex[stateId] = static_cast<uint32_t>(sectionStateIds.size());
															   sectionStateIds.push_back(stateId);
														   }

														   paletteIndices.push_back(stateIdToPaletteIndex[stateId]);
													   }
												   }
											   }

											   std::vector<std::vector<uint8_t>> palElems;
											   palElems.reserve(sectionStateIds.size());
											   for (int32_t stateId : sectionStateIds)
											   {
												   size_t blockIndex = blockRegistry.getBlockIndexFromStateId(stateId);
												   const auto &block = blockRegistry.getBlock(blockIndex);

												   std::string blockName = block.name;
												   if (!isValidMinecraftBlockName(blockName))
												   {
													   if (blockName.substr(0, 10) != "minecraft:")
														   blockName = "minecraft:" + blockName;
												   }

												   palElems.push_back(NBT::buildCompound([&blockName, &block](std::vector<uint8_t> &p)
																						 {
                    NBT::writeTAG_String(p, "Name", blockName);

                    if (!block.properties.empty())
                    {
                        p.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
                        NBT::writeTagName(p, "Properties");
                        for (const auto &[key, value] : block.properties)
                        {
                            if (!key.empty() && !value.empty())
                                NBT::writeTAG_String(p, key, value);
                        }
                        p.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));
                    } }));
											   }

											   NBT::writeTAG_List_of_Compound(c, "palette", palElems);

											   if (sectionStateIds.size() > 1)
											   {
												   auto packed = packBlockStates(paletteIndices, static_cast<int>(sectionStateIds.size()));
												   NBT::writeTAG_LongArray(c, "data", packed);
											   }

											   c.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

											   c.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
											   NBT::writeTagName(c, "biomes");

											   BiomeType chunkBiome = generator.getBiomeForChunk(cx, cz);
											   const auto &biome = generator.biomeGen.getBiome(chunkBiome);

											   std::string biomeName = biome.name;
											   if (biomeName.substr(0, 10) != "minecraft:")
												   biomeName = "minecraft:" + biomeName;

											   std::vector<std::string> bp = {biomeName};
											   NBT::writeTAG_List_of_String(c, "palette", bp);
											   c.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));
										   });

			sectionElements.push_back(std::move(comp));
		}

		NBT::writeTAG_List_of_Compound(sectionsData, "sections", sectionElements);
		sectionsData.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));

		return sectionsData;
	}

	std::vector<uint8_t> Chunk::buildSectionsNetwork(
		int cx, int cz,
		TerrainGenerator &generator,
		BlockRegistry &blockRegistry)
	{
		std::vector<uint8_t> buffer;

		const int minSection = TerrainGenerator::WORLD_HEIGHT_MIN / TerrainGenerator::SECTION_HEIGHT;
		const int maxSection = TerrainGenerator::WORLD_HEIGHT_MAX / TerrainGenerator::SECTION_HEIGHT;

		for (int sY = minSection; sY < maxSection; ++sY)
		{
			std::unordered_map<int32_t, uint32_t> stateIdToPaletteIndex;
			std::vector<int32_t> sectionStateIds;
			std::vector<uint32_t> paletteIndices;
			paletteIndices.reserve(4096);

			int nonAirCount = 0;

			for (int y = 0; y < TerrainGenerator::SECTION_HEIGHT; ++y)
			{
				int worldY = sY * TerrainGenerator::SECTION_HEIGHT + y;
				for (int z = 0; z < TerrainGenerator::CHUNK_SIZE; ++z)
				{
					int worldZ = cz * TerrainGenerator::CHUNK_SIZE + z;
					for (int x = 0; x < TerrainGenerator::CHUNK_SIZE; ++x)
					{
						int worldX = cx * TerrainGenerator::CHUNK_SIZE + x;

						size_t blockIndex = getSafeBlockIndex(
							generator.getBlockAt(worldX, worldY, worldZ),
							blockRegistry);

						int32_t stateId = blockRegistry.getDefaultStateId(blockIndex);

						if (stateId != 0)
							nonAirCount++;

						auto it = stateIdToPaletteIndex.find(stateId);
						if (it == stateIdToPaletteIndex.end())
						{
							stateIdToPaletteIndex[stateId] = (uint32_t)sectionStateIds.size();
							sectionStateIds.push_back(stateId);
						}

						paletteIndices.push_back(stateIdToPaletteIndex[stateId]);
					}
				}
			}


			writeInt16(buffer, (int16_t)nonAirCount);

			if (sectionStateIds.size() == 1)
			{
				writeVarInt(buffer, 0);
				writeVarInt(buffer, 0);
			}
			else
			{
				writeVarInt(buffer, (int32_t)sectionStateIds.size());
				for (int32_t stateId : sectionStateIds)
					writeVarInt(buffer, stateId);

				auto packed = packBlockStates(paletteIndices, (int)sectionStateIds.size());
				writeVarInt(buffer, (int32_t)packed.size());
				for (int64_t v : packed)
					writeLong(buffer, v);
			}
		}

		return buffer;
	}

	ChunkComponents Chunk::buildChunkComponents(int cx, int cz, TerrainGenerator &generator, BlockRegistry &blockRegistry)
	{
		ChunkComponents components;

		components.heightmaps = buildHeightmapsNBT(cx, cz, generator);
		components.sections = buildSectionsNetwork(cx, cz, generator, blockRegistry);

		components.blockEntities.push_back(static_cast<uint8_t>(NBTTagType::TAG_List));
		NBT::writeTagName(components.blockEntities, "block_entities");
		components.blockEntities.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		NBT::append_be32(components.blockEntities, 0);

		return components;
	}

	std::vector<int64_t> Chunk::packHeightmap(const std::vector<int> &heights)
	{
		if (heights.size() != 256)
		{
			std::cerr << "ERROR: Expected 256 heights, got " << heights.size() << std::endl;
			return std::vector<int64_t>(36, 0);
		}

		std::vector<int64_t> packed(36, 0);
		const int bitsPerValue = 9;
		const int valuesPerLong = 64 / bitsPerValue;

		for (int i = 0; i < 256; i++)
		{
			int heightValue = std::clamp(heights[i], 0, 511);

			int longIndex = i / valuesPerLong;
			int bitOffset = (i % valuesPerLong) * bitsPerValue;

			if (bitOffset + bitsPerValue <= 64)
				packed[longIndex] |= (static_cast<int64_t>(heightValue) << bitOffset);
			else
			{
				int bitsInFirst = 64 - bitOffset;
				int bitsInSecond = bitsPerValue - bitsInFirst;

				packed[longIndex] |= (static_cast<int64_t>(heightValue) << bitOffset);
				if (longIndex + 1 < 36)
				{
					packed[longIndex + 1] |= (static_cast<int64_t>(heightValue) >> bitsInFirst);
				}
			}
		}

		return packed;
	}

	std::vector<int64_t> Chunk::packBlockStates(const std::vector<uint32_t> &indices, int paletteSize)
	{
		int bitsPerBlock = std::max(4, (int)std::ceil(std::log2(paletteSize)));
		int valuesPerLong = 64 / bitsPerBlock;
		int64_t mask = (1LL << bitsPerBlock) - 1;

		int totalLongs = (indices.size() + valuesPerLong - 1) / valuesPerLong;
		std::vector<int64_t> packed(totalLongs, 0);

		int longIndex = 0;
		int bitOffset = 0;

		for (uint32_t val : indices)
		{
			packed[longIndex] |= (static_cast<int64_t>(val) & mask) << bitOffset;
			bitOffset += bitsPerBlock;

			if (bitOffset + bitsPerBlock > 64)
			{
				longIndex++;
				bitOffset = 0;
			}
		}

		return packed;
	}

	bool Chunk::isValidBlockIndex(size_t blockIndex, const BlockRegistry &blockRegistry)
	{
		return blockIndex < blockRegistry.count();
	}

	size_t Chunk::getSafeBlockIndex(size_t blockIndex, const BlockRegistry &blockRegistry)
	{
		if (isValidBlockIndex(blockIndex, blockRegistry))
			return blockIndex;
		std::cerr << "WARNING: Invalid block index " << blockIndex << ", using air" << std::endl;
		return 0;
	}

	bool Chunk::isValidMinecraftBlockName(const std::string &blockName)
	{
		if (blockName.substr(0, 10) != "minecraft:")
			return false;

		std::string name = blockName.substr(10);
		if (name.empty())
			return false;

		for (char c : name)
		{
			if (!std::islower(c) && c != '_' && !std::isdigit(c))
				return false;
		}
		return true;
	}

	void Chunk::writeNBTCompound(std::vector<uint8_t> &data, const std::string &name)
	{
		data.push_back(0x0A);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());
	}

	void Chunk::writeNBTList(std::vector<uint8_t> &data, const std::string &name, uint8_t elementType, int32_t elementCount)
	{
		data.push_back(0x09);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());
		data.push_back(elementType);

		data.push_back((elementCount >> 24) & 0xFF);
		data.push_back((elementCount >> 16) & 0xFF);
		data.push_back((elementCount >> 8) & 0xFF);
		data.push_back(elementCount & 0xFF);
	}

	void Chunk::writeNBTByte(std::vector<uint8_t> &data, const std::string &name, uint8_t value)
	{
		data.push_back(0x01);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());
		data.push_back(value);
	}

	void Chunk::writeNBTInt(std::vector<uint8_t> &data, const std::string &name, int32_t value)
	{
		data.push_back(0x03);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());
		data.push_back((value >> 24) & 0xFF);
		data.push_back((value >> 16) & 0xFF);
		data.push_back((value >> 8) & 0xFF);
		data.push_back(value & 0xFF);
	}

	void Chunk::writeNBTLong(std::vector<uint8_t> &data, const std::string &name, int64_t value)
	{
		data.push_back(0x04);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());
		for (int i = 7; i >= 0; --i)
		{
			data.push_back((value >> (i * 8)) & 0xFF);
		}
	}

	void Chunk::writeNBTString(std::vector<uint8_t> &data, const std::string &name, const std::string &value)
	{
		data.push_back(0x08);
		uint16_t nameLength = static_cast<uint16_t>(name.size());
		data.push_back((nameLength >> 8) & 0xFF);
		data.push_back(nameLength & 0xFF);
		data.insert(data.end(), name.begin(), name.end());

		uint16_t valueLength = static_cast<uint16_t>(value.size());
		data.push_back((valueLength >> 8) & 0xFF);
		data.push_back(valueLength & 0xFF);
		data.insert(data.end(), value.begin(), value.end());
	}

	void Chunk::writeNBTEnd(std::vector<uint8_t> &data)
	{
		data.push_back(0x00);
	}
}