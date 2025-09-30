#include "Courage/World/Region.hpp"
#include <iostream>

namespace Courage::World
{
	RegionFile::RegionFile(const std::string &path) : path(path)
	{
		std::filesystem::create_directories(std::filesystem::path(path).parent_path());

		if (!std::filesystem::exists(path))
			createEmptyRegionFile();
	}

	bool RegionFile::hasChunk(int cx, int cz)
	{
		std::lock_guard<std::mutex> lock(writeMutex);
		return chunkExists(cx, cz);
	}

	std::vector<uint8_t> RegionFile::readChunk(int cx, int cz)
	{
		std::lock_guard<std::mutex> lock(writeMutex);

		if (!chunkExists(cx, cz))
			return {};

		auto [offset, sectors] = getChunkLocation(cx, cz);
		if (offset == 0 || sectors == 0)
			return {};

		std::ifstream f(path, std::ios::binary);
		if (!f)
			throw std::runtime_error("Cannot open region file for reading: " + path);

		f.seekg(offset);

		uint32_t chunkLength;
		uint8_t compressionType;

		f.read(reinterpret_cast<char *>(&chunkLength), 4);
		f.read(reinterpret_cast<char *>(&compressionType), 1);

		chunkLength = ((chunkLength & 0xFF000000) >> 24) |
					  ((chunkLength & 0x00FF0000) >> 8) |
					  ((chunkLength & 0x0000FF00) << 8) |
					  ((chunkLength & 0x000000FF) << 24);

		if (chunkLength == 0 || chunkLength > sectors * 4096)
		{
			std::cerr << "Invalid chunk length: " << chunkLength << std::endl;
			return {};
		}

		std::vector<uint8_t> compressed(chunkLength - 1);
		f.read(reinterpret_cast<char *>(compressed.data()), chunkLength - 1);

		if (compressionType != 2)
			throw std::runtime_error("Unsupported compression type: " + std::to_string(compressionType));

		return decompress(compressed);
	}

	void RegionFile::writeChunk(int cx, int cz, const std::vector<uint8_t> &data)
	{
		std::lock_guard<std::mutex> lock(writeMutex);

		auto compressed = compress(data);

		uint32_t totalSize = 5 + compressed.size();
		uint32_t sectorsNeeded = (totalSize + 4095) / 4096;

		auto [offset, allocatedSize] = allocateChunkSpace(cx, cz, sectorsNeeded * 4096);

		std::fstream f(path, std::ios::in | std::ios::out | std::ios::binary);
		if (!f)
			throw std::runtime_error("Cannot open region file for writing: " + path);
		f.seekp(offset);

		uint32_t lengthBE = ((compressed.size() + 1) & 0xFF000000) >> 24 |
							((compressed.size() + 1) & 0x00FF0000) >> 8 |
							((compressed.size() + 1) & 0x0000FF00) << 8 |
							((compressed.size() + 1) & 0x000000FF) << 24;
		f.write(reinterpret_cast<char *>(&lengthBE), 4);

		uint8_t compressionType = 2;
		f.write(reinterpret_cast<char *>(&compressionType), 1);
		f.write(reinterpret_cast<const char *>(compressed.data()), compressed.size());
		updateChunkHeader(cx, cz, offset / 4096, sectorsNeeded);
		updateChunkTimestamp(cx, cz, static_cast<uint32_t>(std::time(nullptr)));
	}

	void RegionFile::createEmptyRegionFile()
	{
		std::ofstream f(path, std::ios::binary);
		if (!f)
			throw std::runtime_error("Cannot create region file: " + path);

		std::vector<uint8_t> emptyHeaders(8192, 0);
		f.write(reinterpret_cast<char *>(emptyHeaders.data()), emptyHeaders.size());
	}

	bool RegionFile::chunkExists(int cx, int cz)
	{
		auto [offset, sectors] = getChunkLocation(cx, cz);
		return offset != 0 && sectors != 0;
	}

	std::pair<uint32_t, uint8_t> RegionFile::getChunkLocation(int cx, int cz)
	{
		std::ifstream f(path, std::ios::binary);
		if (!f)
			return {0, 0};

		int localX = cx & 31;
		int localZ = cz & 31;
		int index = localX + localZ * 32;

		f.seekg(index * 4);
		uint8_t buf[4];
		f.read(reinterpret_cast<char *>(buf), 4);

		if (!f.good())
			return {0, 0};

		uint32_t offset = (static_cast<uint32_t>(buf[0]) << 16) |
						  (static_cast<uint32_t>(buf[1]) << 8) |
						  static_cast<uint32_t>(buf[2]);
		uint8_t sectors = buf[3];

		offset *= 4096;

		return {offset, sectors};
	}

	std::pair<uint32_t, uint32_t> RegionFile::allocateChunkSpace(int cx, int cz, size_t size)
	{
		uint32_t fileSize = 0;
		if (std::filesystem::exists(path))
			fileSize = static_cast<uint32_t>(std::filesystem::file_size(path));
		else
			fileSize = 8192;

		uint32_t alignedSize = ((size + 4095) / 4096) * 4096;

		return {fileSize, alignedSize};
	}

	void RegionFile::updateChunkHeader(int cx, int cz, uint32_t sectorOffset, uint8_t sectorCount)
	{
		std::fstream f(path, std::ios::in | std::ios::out | std::ios::binary);
		if (!f)
			throw std::runtime_error("Cannot update chunk header in: " + path);

		int localX = cx & 31;
		int localZ = cz & 31;
		int index = localX + localZ * 32;

		f.seekp(index * 4);

		uint8_t buf[4];
		buf[0] = (sectorOffset >> 16) & 0xFF;
		buf[1] = (sectorOffset >> 8) & 0xFF;
		buf[2] = sectorOffset & 0xFF;
		buf[3] = sectorCount;

		f.write(reinterpret_cast<char *>(buf), 4);
	}

	void RegionFile::updateChunkTimestamp(int cx, int cz, uint32_t timestamp)
	{
		std::fstream f(path, std::ios::in | std::ios::out | std::ios::binary);
		if (!f)
			return;

		int localX = cx & 31;
		int localZ = cz & 31;
		int index = localX + localZ * 32;

		f.seekp(4096 + index * 4);

		uint8_t buf[4];
		buf[0] = (timestamp >> 24) & 0xFF;
		buf[1] = (timestamp >> 16) & 0xFF;
		buf[2] = (timestamp >> 8) & 0xFF;
		buf[3] = timestamp & 0xFF;

		f.write(reinterpret_cast<char *>(buf), 4);
	}

	std::vector<uint8_t> RegionFile::compress(const std::vector<uint8_t> &data)
	{
		uLongf compressedSize = compressBound(data.size());
		std::vector<uint8_t> out(compressedSize);

		int result = ::compress2(out.data(), &compressedSize, data.data(), data.size(), Z_BEST_SPEED);
		if (result != Z_OK)
			throw std::runtime_error("Compression error: " + std::to_string(result));

		out.resize(compressedSize);
		return out;
	}

	std::vector<uint8_t> RegionFile::decompress(const std::vector<uint8_t> &compressed)
	{
		const size_t initialMultiplier = 20;
		const size_t maxSize = 10 * 1024 * 1024;

		for (size_t multiplier = initialMultiplier; multiplier <= 200; multiplier *= 2)
		{
			uLongf outSize = compressed.size() * multiplier;

			if (outSize > maxSize)
				throw std::runtime_error("Decompressed size would be too large: " + std::to_string(outSize));

			std::vector<uint8_t> out(outSize);
			int result = ::uncompress(out.data(), &outSize, compressed.data(), compressed.size());

			if (result == Z_OK)
			{
				out.resize(outSize);
				return out;
			}

			if (result != Z_BUF_ERROR)
				throw std::runtime_error("Decompression error: " + std::to_string(result));
		}

		throw std::runtime_error("Could not decompress even with large buffer");
	}
}