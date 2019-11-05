#pragma once

#include <cstdint>

typedef uint8_t byte;
static const byte byte_mask = 0b11111111;
static const int byte_bits = 8;

typedef int bignum;
static const bignum byte_values = 256;

using two_bytes_bytes = uint16_t;
union two_bytes {
	// inline explicit two_bytes(two_bytes_bytes bb) : bytes(bb) {};
	// inline explicit two_bytes(byte b1, byte b2) : byte_1(b1), byte_2(b2) {};
	// inline explicit operator two_bytes_bytes() { return bytes; }

	two_bytes_bytes bytes;
	struct {
		byte byte_1;
		byte byte_2;
	};
};

using two_two_bytes_bytes = uint32_t;
union two_two_bytes {
	// inline explicit two_two_bytes(two_two_bytes_bytes bbbb) : bytes(bbbb) {};
	// inline explicit two_two_bytes(two_bytes_bytes bb1, two_bytes_bytes bb2) : two_bytes_1(bb1), two_bytes_2(bb2) {};
	// inline explicit operator two_two_bytes_bytes() { return bytes; }

	two_two_bytes_bytes bytes;
	struct {
		two_bytes two_bytes_1;
		two_bytes two_bytes_2;
	};
};

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
