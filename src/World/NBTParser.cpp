#include "Courage/World/NBTParser.hpp"

namespace Courage::World
{

	int16_t NBTParser::readInt16()
	{
		if (pos + 2 > size)
			throw std::runtime_error("NBT: Unexpected end of data");
		int16_t val = (static_cast<int16_t>(data[pos]) << 8) | data[pos + 1];
		pos += 2;
		return val;
	}

	int32_t NBTParser::readInt32()
	{
		if (pos + 4 > size)
			throw std::runtime_error("NBT: Unexpected end of data");
		int32_t val = (static_cast<int32_t>(data[pos]) << 24) |
					  (static_cast<int32_t>(data[pos + 1]) << 16) |
					  (static_cast<int32_t>(data[pos + 2]) << 8) |
					  static_cast<int32_t>(data[pos + 3]);
		pos += 4;
		return val;
	}

	int64_t NBTParser::readInt64()
	{
		if (pos + 8 > size)
			throw std::runtime_error("NBT: Unexpected end of data");
		int64_t val = 0;
		for (int i = 0; i < 8; i++)
		{
			val = (val << 8) | data[pos + i];
		}
		pos += 8;
		return val;
	}

	std::string NBTParser::readString()
	{
		int16_t len = readInt16();
		if (len < 0 || pos + len > size)
			throw std::runtime_error("NBT: Invalid string length");
		std::string str(reinterpret_cast<const char *>(data + pos), len);
		pos += len;
		return str;
	}

	std::unique_ptr<NBTValue> NBTParser::parseValue(NBTTagType type)
	{
		switch (type)
		{
		case NBTTagType::TAG_End:
			return nullptr;

		case NBTTagType::TAG_Byte:
			if (pos >= size)
				throw std::runtime_error("NBT: Unexpected end of data");
			return std::make_unique<NBTByte>(static_cast<int8_t>(data[pos++]));

		case NBTTagType::TAG_Short:
			return std::make_unique<NBTShort>(readInt16());

		case NBTTagType::TAG_Int:
			return std::make_unique<NBTInt>(readInt32());

		case NBTTagType::TAG_Long:
			return std::make_unique<NBTLong>(readInt64());

		case NBTTagType::TAG_String:
			return std::make_unique<NBTString>(readString());

		case NBTTagType::TAG_Byte_Array:
		{
			int32_t len = readInt32();
			if (len < 0 || pos + len > size)
				throw std::runtime_error("NBT: Invalid byte array length");
			std::vector<int8_t> arr(len);
			std::memcpy(arr.data(), data + pos, len);
			pos += len;
			return std::make_unique<NBTByteArray>(arr);
		}

		case NBTTagType::TAG_Int_Array:
		{
			int32_t len = readInt32();
			if (len < 0 || pos + len * 4 > size)
				throw std::runtime_error("NBT: Invalid int array length");
			std::vector<int32_t> arr(len);
			for (int32_t i = 0; i < len; i++)
			{
				arr[i] = readInt32();
			}
			return std::make_unique<NBTIntArray>(arr);
		}

		case NBTTagType::TAG_Long_Array:
		{
			int32_t len = readInt32();
			if (len < 0 || pos + len * 8 > size)
				throw std::runtime_error("NBT: Invalid long array length");
			std::vector<int64_t> arr(len);
			for (int32_t i = 0; i < len; i++)
			{
				arr[i] = readInt64();
			}
			return std::make_unique<NBTLongArray>(arr);
		}

		case NBTTagType::TAG_List:
		{
			NBTTagType elementType = static_cast<NBTTagType>(data[pos++]);
			int32_t len = readInt32();
			if (len < 0)
				throw std::runtime_error("NBT: Invalid list length");

			auto list = std::make_unique<NBTList>(elementType);
			for (int32_t i = 0; i < len; i++)
			{
				list->value.push_back(parseValue(elementType));
			}
			return list;
		}

		case NBTTagType::TAG_Compound:
		{
			auto compound = std::make_unique<NBTCompound>();

			while (pos < size)
			{
				NBTTagType tagType = static_cast<NBTTagType>(data[pos++]);
				if (tagType == NBTTagType::TAG_End)
					break;

				std::string name = readString();
				auto value = parseValue(tagType);
				compound->value[name] = std::move(value);
			}

			return compound;
		}

		default:
			throw std::runtime_error("NBT: Unknown tag type");
		}
	}

	std::unique_ptr<NBTCompound> NBTParser::parse(const std::vector<uint8_t> &nbtData)
	{
		data = nbtData.data();
		size = nbtData.size();
		pos = 0;

		if (size == 0)
			return nullptr;

		NBTTagType rootType = static_cast<NBTTagType>(data[pos++]);
		if (rootType != NBTTagType::TAG_Compound)
		{
			throw std::runtime_error("NBT: Root tag must be compound");
		}

		std::string rootName = readString();
		return std::unique_ptr<NBTCompound>(dynamic_cast<NBTCompound *>(parseValue(rootType).release()));
	}
}