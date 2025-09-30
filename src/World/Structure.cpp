#include <Courage/World/Structure.hpp>

namespace Courage::World
{
	StructureGenerator::StructureGenerator(uint64_t seed, const std::vector<size_t> &wood, const std::vector<size_t> &leaves, const std::vector<size_t> &stones)
		: structNoise(seed + 3000), woodBlocks(wood), leafBlocks(leaves), stoneBlocks(stones) {}

	std::vector<Structure> StructureGenerator::generateStructuresInChunk(int cx, int cz, BiomeType biome)
	{
		std::vector<Structure> structures;

		float treeNoise = structNoise.detailNoise(cx * 16, cz * 16, 0.1f);
		if (treeNoise > 0.7f && !woodBlocks.empty() && !leafBlocks.empty())
		{
			Structure tree;
			tree.type = Structure::TREE;
			tree.x = cx * 16 + 8;
			tree.z = cz * 16 + 8;
			tree.y = 70;
			tree.size = 5 + (treeNoise * 3);

			for (int y = 0; y < tree.size; y++)
			{
				tree.blocks.emplace_back(0, y, 0, woodBlocks[0]);
			}

			int leafRadius = 2;
			for (int dy = -1; dy <= 2; dy++)
			{
				for (int dx = -leafRadius; dx <= leafRadius; dx++)
				{
					for (int dz = -leafRadius; dz <= leafRadius; dz++)
					{
						if (dx * dx + dz * dz <= leafRadius * leafRadius)
							tree.blocks.emplace_back(dx, tree.size + dy, dz, leafBlocks[0]);
					}
				}
			}
			structures.push_back(std::move(tree));
		}

		float dungeonNoise = structNoise.detailNoise(cx * 13, cz * 17, 0.05f);
		if (dungeonNoise > 0.9f && !stoneBlocks.empty())
		{
			Structure dungeon;
			dungeon.type = Structure::DUNGEON;
			dungeon.x = cx * 16 + 8;
			dungeon.z = cz * 16 + 8;
			dungeon.y = 30;
			dungeon.size = 7;

			for (int x = 0; x < 7; x++)
			{
				for (int z = 0; z < 7; z++)
				{
					for (int y = 0; y < 4; y++)
					{
						bool isWall = (x == 0 || x == 6 || z == 0 || z == 6 || y == 0 || y == 3);
						if (isWall)
							dungeon.blocks.emplace_back(x - 3, y, z - 3, stoneBlocks[0]);
					}
				}
			}
			structures.push_back(std::move(dungeon));
		}

		return structures;
	}
}