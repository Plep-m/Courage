#pragma once
#include <zlib.h>
#include <vector>
#include <string>
#include <functional>
#include <stdexcept>
#include <memory>
#include <unordered_map>

namespace Courage::World
{
	class NBT
	{
	public:
		static void append_be8(std::vector<uint8_t> &out, uint8_t v);
		static void append_be16(std::vector<uint8_t> &out, uint16_t v);
		static void append_be32(std::vector<uint8_t> &out, uint32_t v);
		static void append_be64_signed(std::vector<uint8_t> &out, int64_t v);

		static std::vector<uint8_t> compress(const std::vector<uint8_t> &src);

		static void writeTagName(std::vector<uint8_t> &out, const std::string &name);
		static void writeTAG_Int(std::vector<uint8_t> &out, const std::string &name, int32_t v);
		static void writeTAG_Long(std::vector<uint8_t> &out, const std::string &name, int64_t v);
		static void writeTAG_String(std::vector<uint8_t> &out, const std::string &name, const std::string &s);
		static void writeTAG_LongArray(std::vector<uint8_t> &out, const std::string &name, const std::vector<int64_t> &arr);
		static void writeTAG_List_of_Compound(std::vector<uint8_t> &out, const std::string &name, const std::vector<std::vector<uint8_t>> &elements);
		static void writeTAG_List_of_String(std::vector<uint8_t> &out, const std::string &name, const std::vector<std::string> &elements);
		static void writeTAG_Byte(std::vector<uint8_t> &out, const std::string &name, int8_t v);
		static void writeTAG_Float(std::vector<uint8_t> &out, const std::string &name, float v);
		static void writeTAG_Double(std::vector<uint8_t> &out, const std::string &name, double v);
		static void append_be32_signed(std::vector<uint8_t> &out, int32_t v);
		static void writeTAG_Compound(std::vector<uint8_t> &out, const std::string &name, const std::vector<uint8_t> &compound);

		static std::vector<uint8_t> buildCompound(std::function<void(std::vector<uint8_t> &)> f);
	};

	enum class NBTTagType : uint8_t
	{
		TAG_End = 0,
		TAG_Byte = 1,
		TAG_Short = 2,
		TAG_Int = 3,
		TAG_Long = 4,
		TAG_Float = 5,
		TAG_Double = 6,
		TAG_Byte_Array = 7,
		TAG_String = 8,
		TAG_List = 9,
		TAG_Compound = 10,
		TAG_Int_Array = 11,
		TAG_Long_Array = 12
	};

	class NBTValue
	{
	public:
		virtual ~NBTValue() = default;
		virtual NBTTagType getType() const = 0;
	};

	class NBTByte : public NBTValue
	{
	public:
		int8_t value;
		NBTByte(int8_t v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Byte; }
	};

	class NBTShort : public NBTValue
	{
	public:
		int16_t value;
		NBTShort(int16_t v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Short; }
	};

	class NBTInt : public NBTValue
	{
	public:
		int32_t value;
		NBTInt(int32_t v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Int; }
	};

	class NBTLong : public NBTValue
	{
	public:
		int64_t value;
		NBTLong(int64_t v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Long; }
	};

	class NBTString : public NBTValue
	{
	public:
		std::string value;
		NBTString(const std::string &v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_String; }
	};

	class NBTByteArray : public NBTValue
	{
	public:
		std::vector<int8_t> value;
		NBTByteArray(const std::vector<int8_t> &v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Byte_Array; }
	};

	class NBTIntArray : public NBTValue
	{
	public:
		std::vector<int32_t> value;
		NBTIntArray(const std::vector<int32_t> &v) : value(v) {}
		NBTTagType getType() const override { return NBTTagType::TAG_Int_Array; }
	};

	class NBTLongArray : public NBTValue
	{
	public:
		std::vector<int64_t> value;

		NBTLongArray() = default;
		NBTLongArray(const std::vector<int64_t> &v) : value(v) {}

		NBTTagType getType() const override { return NBTTagType::TAG_Long_Array; }
	};

	class NBTList : public NBTValue
	{
	public:
		NBTTagType elementType;
		std::vector<std::unique_ptr<NBTValue>> value;
		NBTList(NBTTagType type) : elementType(type) {}
		NBTTagType getType() const override { return NBTTagType::TAG_List; }
	};

	class NBTCompound : public NBTValue
	{
	public:
		std::unordered_map<std::string, std::unique_ptr<NBTValue>> value;
		NBTTagType getType() const override { return NBTTagType::TAG_Compound; };

		template <typename T>
		T *get(const std::string &key)
		{
			auto it = value.find(key);
			if (it != value.end())
			{
				return dynamic_cast<T *>(it->second.get());
			}
			return nullptr;
		}

		void set(const std::string &key, std::unique_ptr<NBTValue> val)
		{
			value[key] = std::move(val);
		}

		bool has(const std::string &key) const
		{
			return value.find(key) != value.end();
		}
	};
}