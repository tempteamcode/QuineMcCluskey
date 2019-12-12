#pragma once

#include <cstdint>

extern const char LUT_hex_16[16];
extern char LUT_hex_256[256][4];

extern const unsigned char LUT_bitscount_16[16];
extern unsigned char LUT_bitscount_256[256];

template <typename int_t>
unsigned char bitscount(const int_t value);

template<>
inline unsigned char bitscount<uint8_t>(const uint8_t value)
{
	return LUT_bitscount_256[(value)];
}

template<>
inline unsigned char bitscount<uint16_t>(const uint16_t value)
{
	return LUT_bitscount_256[(value) & 0xFF] + LUT_bitscount_256[(value >> 8)];
}

template<>
inline unsigned char bitscount<uint32_t>(const uint32_t value)
{
	return LUT_bitscount_256[(value) & 0xFF] + LUT_bitscount_256[(value >> 8) & 0xFF] + LUT_bitscount_256[(value >> 16) & 0xFF] + LUT_bitscount_256[(value >> 24)];
}

void init_LUTs();
