#pragma once

extern const char LUT_hex_16[16];
extern char LUT_hex_256[256][4];

extern const unsigned char LUT_bitscount_16[16];
extern unsigned char LUT_bitscount_256[256];

// template <typename int_t>
// inline unsigned char bitscount(const int_t value);
/* {
	const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&value);
	unsigned char result;
	int size = sizeof(int_t);
	while (size --> 0)
	{
		result += LUT_bitscount_256[*ptr++];
	}
	return result;
} */

// template <uint8_t>
inline unsigned char bitscount(const uint8_t value)
{
	return LUT_bitscount_256[(value)];
}

// template <uint16_t>
inline unsigned char bitscount(const uint16_t value)
{
	return LUT_bitscount_256[(value) & 0xFF] + LUT_bitscount_256[(value >> 8)];
}

// template <uint32_t>
inline unsigned char bitscount(const uint32_t value)
{
	return LUT_bitscount_256[(value) & 0xFF] + LUT_bitscount_256[(value >> 8) & 0xFF] + LUT_bitscount_256[(value >> 16) & 0xFF] + LUT_bitscount_256[(value >> 24)];
}

void init_LUTs();
