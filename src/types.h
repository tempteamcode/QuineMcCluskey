#pragma once

#include <cstdint>

typedef uint8_t byte;
static const byte byte_mask = 0b11111111;
static const int byte_bits = 8;

typedef int bignum;
static const bignum byte_values = 256;

union two_bytes {
	uint16_t bytes;
	struct {
		byte byte_1;
		byte byte_2;
	};
};
using two_bytes_bytes = decltype(two_bytes::bytes);

union two_two_bytes {
	uint32_t bytes;
	struct {
		two_bytes two_bytes_1;
		two_bytes two_bytes_2;
	};
};
using two_two_bytes_bytes = decltype(two_two_bytes::bytes);

inline bool operator ==(const two_bytes& lhs, const two_bytes& rhs)
{
	return lhs.bytes == rhs.bytes;
}

inline bool operator <(const two_bytes& lhs, const two_bytes& rhs)
{
	return lhs.bytes < rhs.bytes;
}

inline bool operator ==(const two_two_bytes& lhs, const two_two_bytes& rhs)
{
	return lhs.bytes == rhs.bytes;
}

inline bool operator <(const two_two_bytes& lhs, const two_two_bytes& rhs)
{
	return lhs.bytes < rhs.bytes;
}

#include <map>

namespace std
{
template<>
class hash<two_bytes>
{
public:
    inline size_t operator()(two_bytes const& bytes) const
    {
        return bytes.bytes;
    }
};
template<>
class hash<two_two_bytes>
{
public:
    inline size_t operator()(two_two_bytes const& bytes) const
    {
        return bytes.bytes;
    }
};
}

/*
		: public _Bitwise_hash<char32_t>
	{	// hash functor for char32_t
	};
*/
