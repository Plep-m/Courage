#include "Courage/Network/Chunk.hpp"
#include "Courage/World/NBT.hpp"

static std::vector<int> makeFlatSectionBlocks(int sectionY, Courage::World::BlockRegistry &registry)
{
	std::vector<int> blocks(4096, 0);

	int worldY = sectionY * 16;

	int air = registry.getDefaultStateId(registry.getIndex("minecraft:air"));
	int bed = registry.getDefaultStateId(registry.getIndex("minecraft:bedrock"));
	int dirt = registry.getDefaultStateId(registry.getIndex("minecraft:dirt"));
	int grass = registry.getDefaultStateId(registry.getIndex("minecraft:grass_block"));

	for (int y = 0; y < 16; y++)
	{
		int absY = worldY + y;
		int blockId = air;

		if (absY == 0)
			blockId = bed;
		else if (absY > 0 && absY <= 2)
			blockId = dirt;
		else if (absY == 3)
			blockId = grass;

		for (int z = 0; z < 16; z++)
			for (int x = 0; x < 16; x++)
				blocks[(y * 16 + z) * 16 + x] = blockId;
	}
	return blocks;
}

static void writeBlockStates(std::vector<uint8_t> &buf, const std::vector<int> &blocks)
{
	std::unordered_map<int, int> paletteIndex;
	std::vector<int> palette;
	for (int id : blocks)
	{
		if (paletteIndex.find(id) == paletteIndex.end())
		{
			int idx = palette.size();
			paletteIndex[id] = idx;
			palette.push_back(id);
		}
	}

	int bitsPerEntry = std::max(4, (int)std::ceil(std::log2(std::max(1, (int)palette.size()))));
	if (bitsPerEntry == 0)
		bitsPerEntry = 1;

	Courage::World::NBT::append_be8(buf, bitsPerEntry);

	writeVarInt(buf, (int)palette.size());
	for (int id : palette)
		writeVarInt(buf, id);

	int entries = 4096;
	int valuesPerLong = 64 / bitsPerEntry;
	int arraySize = (entries + valuesPerLong - 1) / valuesPerLong;
	writeVarInt(buf, arraySize);

	uint64_t current = 0;
	int filled = 0;
	int bitIndex = 0;

	for (int i = 0; i < entries; i++)
	{
		int palIdx = paletteIndex[blocks[i]];
		current |= (uint64_t)palIdx << (bitIndex * bitsPerEntry);
		bitIndex++;
		if (bitIndex == valuesPerLong)
		{
			Courage::World::NBT::append_be64_signed(buf, current);
			current = 0;
			bitIndex = 0;
		}
	}
	if (bitIndex > 0)
		Courage::World::NBT::append_be64_signed(buf, current);
}

static inline void writeFixedPaletted(std::vector<uint8_t> &buf, int bitsPerEntry, int paletteVal, int entries)
{
	Courage::World::NBT::append_be8(buf, bitsPerEntry);
	writeVarInt(buf, 1);
	writeVarInt(buf, paletteVal);

	int valuesPerLong = 64 / bitsPerEntry;
	int arraySize = (entries + valuesPerLong - 1) / valuesPerLong;

	writeVarInt(buf, arraySize);
	for (int i = 0; i < arraySize; i++)
		Courage::World::NBT::append_be64_signed(buf, 0);
}

static void writeFlatSection(std::vector<uint8_t> &buf, int sectionY, int biomeId, Courage::World::BlockRegistry &registry)
{
	auto blocks = makeFlatSectionBlocks(sectionY, registry);

	int nonAir = 0;
	for (int b : blocks)
		if (b != registry.getDefaultStateId(registry.getIndex("minecraft:air")))
			nonAir++;
	Courage::World::NBT::append_be16(buf, nonAir);

	writeBlockStates(buf, blocks);

	writeFixedPaletted(buf, 1, biomeId, 64);
}

static std::vector<uint8_t> makeHeightmapNBT()
{
	std::vector<uint8_t> n;
	Courage::World::NBT::append_be8(n, 0x0A);

	Courage::World::NBT::append_be8(n, 0x0C);
	Courage::World::NBT::writeTagName(n, "MOTION_BLOCKING");
	Courage::World::NBT::append_be32(n, 36);
	for (int i = 0; i < 36; ++i)
		Courage::World::NBT::append_be64_signed(n, 0);

	Courage::World::NBT::append_be8(n, 0x0C);
	Courage::World::NBT::writeTagName(n, "WORLD_SURFACE");
	Courage::World::NBT::append_be32(n, 36);
	for (int i = 0; i < 36; ++i)
		Courage::World::NBT::append_be64_signed(n, 0);

	Courage::World::NBT::append_be8(n, 0x00);
	return n;
}

static inline void writeSingleValuePaletted(std::vector<uint8_t> &buf, int32_t valueId)
{
	Courage::World::NBT::append_be8(buf, 0);
	writeVarInt(buf, 1);
	writeVarInt(buf, valueId);
	writeVarInt(buf, 0);
}

static inline void writeEmptySection(std::vector<uint8_t> &buf, int biomeId)
{
	Courage::World::NBT::append_be16(buf, 0);
	writeFixedPaletted(buf, 4, 0, 4096);
	writeFixedPaletted(buf, 1, biomeId, 64);
}

static inline void writeEmptyLight(std::vector<uint8_t> &buf, int sectionCount)
{
	const int lightSections = sectionCount + 2;
	const int longsNeeded = (lightSections + 63) / 64;
	const uint64_t mask = (lightSections >= 64) ? ~0ULL : ((1ULL << lightSections) - 1ULL);

	writeVarInt(buf, longsNeeded);
	Courage::World::NBT::append_be64_signed(buf, mask);
	writeVarInt(buf, longsNeeded);
	Courage::World::NBT::append_be64_signed(buf, mask);

	writeVarInt(buf, 0);
	writeVarInt(buf, 0);

	writeVarInt(buf, lightSections);
	for (int i = 0; i < lightSections; i++)
	{
		writeVarInt(buf, 2048);
		for (int j = 0; j < 2048; j++)
			Courage::World::NBT::append_be8(buf, 0xFF);
	}

	writeVarInt(buf, lightSections);
	for (int i = 0; i < lightSections; i++)
	{
		writeVarInt(buf, 2048);
		for (int j = 0; j < 2048; j++)
			Courage::World::NBT::append_be8(buf, 0x00);
	}
}

std::vector<uint8_t> makeChunkPacket(int chunkX, int chunkZ)
{
	const int sectionCount = 24;
	const int biomeId = 4;

	std::vector<uint8_t> p;
	writeVarInt(p, 0x27);
	Courage::World::NBT::append_be32(p, chunkX);
	Courage::World::NBT::append_be32(p, chunkZ);

	auto nbt = makeHeightmapNBT();
	writeBytes(p, nbt);

	std::vector<uint8_t> sections;
	sections.reserve(sectionCount * 200);

	auto &registry = Courage::World::BlockRegistry::getInstance();
	for (int i = 0; i < sectionCount; ++i)
	{
		int sectionY = i - 4;

		if (sectionY == 0)
			writeFlatSection(sections, sectionY, biomeId, registry);
		else
			writeEmptySection(sections, biomeId);
	}
	writeVarInt(p, (int32_t)sections.size());
	writeBytes(p, sections);

	writeVarInt(p, 0);

	writeEmptyLight(p, sectionCount);

	return p;
}