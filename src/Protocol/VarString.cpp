#include "Courage/Protocol/VarString.hpp"

namespace Courage::Protocol
{
	std::string readVarString(const std::vector<uint8_t> &data, size_t &offset)
	{
		int length = 0;
		int shift = 0;
		while (true)
		{
			if (offset >= data.size())
				throw std::runtime_error("readMCString: Out of bounds");
			uint8_t byte = data[offset++];
			length |= (byte & 0x7F) << shift;
			if (!(byte & 0x80))
				break;
			shift += 7;
			if (shift > 35)
				throw std::runtime_error("readMCString: VarInt too big");
		}

		if (offset + length > data.size())
			throw std::runtime_error("readMCString: Not enough data");
		std::string result(data.begin() + offset, data.begin() + offset + length);
		offset += length;
		return result;
	}

	void writeVarString(std::vector<uint8_t> &out, const std::string &str)
	{
		int length = str.size();
		while (true)
		{
			if ((length & ~0x7F) == 0)
			{
				out.push_back(length);
				break;
			}
			else
			{
				out.push_back((length & 0x7F) | 0x80);
				length >>= 7;
			}
		}
		out.insert(out.end(), str.begin(), str.end());
	}
}