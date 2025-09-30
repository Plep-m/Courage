#include "Courage/World/NBT.hpp"
#include <cstring>
#include <thread>

namespace Courage::World
{

	class NBTParser
	{
	private:
		const uint8_t *data;
		size_t size;
		size_t pos;

		int16_t readInt16();
		int32_t readInt32();
		int64_t readInt64();
		std::string readString();
		std::unique_ptr<NBTValue> parseValue(NBTTagType type);

	public:
		std::unique_ptr<NBTCompound> parse(const std::vector<uint8_t> &nbtData);
	};
}