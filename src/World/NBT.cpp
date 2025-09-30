#include "Courage/World/NBT.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>

namespace Courage::World
{
	void NBT::append_be8(std::vector<uint8_t> &out, uint8_t v)
	{
		out.push_back(v);
	}
	
	void NBT::append_be32(std::vector<uint8_t> &out, uint32_t v)
	{
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}

	void NBT::append_be16(std::vector<uint8_t> &out, uint16_t v)
	{
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}

	void NBT::append_be64_signed(std::vector<uint8_t> &out, int64_t v)
	{
		uint64_t uv = static_cast<uint64_t>(v);
		for (int i = 7; i >= 0; i--)
			out.push_back((uv >> (i * 8)) & 0xFF);
	}

	std::vector<uint8_t> NBT::compress(const std::vector<uint8_t> &src)
	{
		uLongf bound = compressBound(src.size());
		std::vector<uint8_t> dst(bound);
		int r = compress2(dst.data(), &bound, src.data(), src.size(), Z_BEST_SPEED);
		if (r != Z_OK)
			throw std::runtime_error("Compression failed");
		dst.resize(bound);
		return dst;
	}

	void NBT::writeTagName(std::vector<uint8_t> &out, const std::string &name)
	{
		append_be16(out, static_cast<uint16_t>(name.size()));
		out.insert(out.end(), name.begin(), name.end());
	}

	void NBT::writeTAG_Int(std::vector<uint8_t> &out, const std::string &name, int32_t v)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Int));
		writeTagName(out, name);
		append_be32(out, static_cast<uint32_t>(v));
	}

	void NBT::writeTAG_Long(std::vector<uint8_t> &out, const std::string &name, int64_t v)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Long));
		writeTagName(out, name);
		append_be64_signed(out, v);
	}

	void NBT::writeTAG_String(std::vector<uint8_t> &out, const std::string &name, const std::string &s)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_String));
		writeTagName(out, name);
		append_be16(out, static_cast<uint16_t>(s.size()));
		out.insert(out.end(), s.begin(), s.end());
	}

	void NBT::writeTAG_LongArray(std::vector<uint8_t> &out, const std::string &name, const std::vector<int64_t> &arr)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Long_Array));
		writeTagName(out, name);
		append_be32(out, static_cast<uint32_t>(arr.size()));
		for (int64_t v : arr)
			append_be64_signed(out, v);
	}

	void NBT::writeTAG_List_of_Compound(std::vector<uint8_t> &out, const std::string &name, const std::vector<std::vector<uint8_t>> &elements)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_List));
		writeTagName(out, name);
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		append_be32(out, static_cast<uint32_t>(elements.size()));
		for (const auto &elem : elements)
		{
			out.insert(out.end(), elem.begin(), elem.end());
		}
	}

	void NBT::writeTAG_List_of_String(std::vector<uint8_t> &out, const std::string &name, const std::vector<std::string> &elements)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_List));
		writeTagName(out, name);
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_String));
		append_be32(out, static_cast<uint32_t>(elements.size()));
		for (const auto &s : elements)
		{
			append_be16(out, static_cast<uint16_t>(s.size()));
			out.insert(out.end(), s.begin(), s.end());
		}
	}

	std::vector<uint8_t> NBT::buildCompound(std::function<void(std::vector<uint8_t> &)> f)
	{
		std::vector<uint8_t> buf;
		f(buf);
		buf.push_back(static_cast<uint8_t>(NBTTagType::TAG_End));
		return buf;
	}

	void NBT::append_be32_signed(std::vector<uint8_t> &out, int32_t v)
	{
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}

	void NBT::writeTAG_Byte(std::vector<uint8_t> &out, const std::string &name, int8_t v)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Byte));
		writeTagName(out, name);
		out.push_back(static_cast<uint8_t>(v));
	}

	void NBT::writeTAG_Float(std::vector<uint8_t> &out, const std::string &name, float v)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Float));
		writeTagName(out, name);
		uint32_t raw;
		std::memcpy(&raw, &v, sizeof(float));
		append_be32(out, raw);
	}

	void NBT::writeTAG_Double(std::vector<uint8_t> &out, const std::string &name, double v)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Double));
		writeTagName(out, name);
		uint64_t raw;
		std::memcpy(&raw, &v, sizeof(double));
		for (int i = 7; i >= 0; --i)
			out.push_back((raw >> (i * 8)) & 0xFF);
	}

	void NBT::writeTAG_Compound(std::vector<uint8_t> &out, const std::string &name, const std::vector<uint8_t> &compound)
	{
		out.push_back(static_cast<uint8_t>(NBTTagType::TAG_Compound));
		writeTagName(out, name);
		out.insert(out.end(), compound.begin(), compound.end());
	}
}