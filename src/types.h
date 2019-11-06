#pragma once

#include <cstdint>


union pair8bits {
	uint16_t bytes;
	struct {
		uint8_t first;
		uint8_t second;
	};
};

union pair16bits {
	uint32_t bytes;
	struct {
		uint16_t first;
		uint16_t second;
	};
};


struct with8bits {
	static const int bits_nb = 8;

	using byte = uint8_t;
	static const byte byte_mask = 0b11111111;

	using big = unsigned int;
	static const big byte_values = 256;

	using pair = pair8bits;
};

struct with16bits {
	static const int bits_nb = 16;

	using byte = uint16_t;
	static const byte byte_mask = 0b1111111111111111;

	using big = unsigned int;
	static const big byte_values = 65536;

	using pair = pair16bits;
};


inline bool operator ==(const pair8bits& lhs, const pair8bits& rhs)
{
	return lhs.bytes == rhs.bytes;
}
inline bool operator <(const pair8bits& lhs, const pair8bits& rhs)
{
	return lhs.bytes < rhs.bytes;
}

inline bool operator ==(const pair16bits& lhs, const pair16bits& rhs)
{
	return lhs.bytes == rhs.bytes;
}
inline bool operator <(const pair16bits& lhs, const pair16bits& rhs)
{
	return lhs.bytes < rhs.bytes;
}


#include <map>

namespace std
{

template<>
class hash<pair8bits>
{
public:
	inline size_t operator()(pair8bits const& bytes) const
	{
		return bytes.bytes;
	}
};
template<>
class hash<pair16bits>
{
public:
	inline size_t operator()(pair16bits const& bytes) const
	{
		return bytes.bytes;
	}
};

}
