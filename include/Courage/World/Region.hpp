#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <zlib.h>
#include <filesystem>
#include <stdexcept>

namespace Courage::World
{
	class RegionFile
	{
	public:
		RegionFile(const std::string &path);
		bool hasChunk(int cx, int cz);
		std::vector<uint8_t> readChunk(int cx, int cz);
		void writeChunk(int cx, int cz, const std::vector<uint8_t> &data);

	private:
		std::string path;
		std::mutex writeMutex;

		bool chunkExists(int cx, int cz);

		std::pair<uint32_t, uint8_t> getChunkLocation(int cx, int cz);
		std::pair<uint32_t, uint32_t> allocateChunkSpace(int cx, int cz, size_t size);
		void createEmptyRegionFile();
		void updateChunkTimestamp(int cx, int cz, uint32_t timestamp);
		void updateChunkHeader(int cx, int cz, uint32_t sectorOffset, uint8_t sectorCount);
		std::vector<uint8_t> compress(const std::vector<uint8_t> &data);
		std::vector<uint8_t> decompress(const std::vector<uint8_t> &compressed);
	};
}