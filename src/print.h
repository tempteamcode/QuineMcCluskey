#pragma once

#include <iostream>
#include <iomanip>
#include <bitset>

#include "types.h"
#include "LUTs.h"
#include "Tables.h"
#include "Charts.h"

template<typename int_t>
inline void print_hex(std::ostream& out, int_t value)
{
	if (sizeof(int_t) == 1)
	{
		out << LUT_hex_256[(unsigned char) value];
//		out << LUT_hex_16[(((unsigned char) value) & 0b11110000) >> 8]
//			<< LUT_hex_16[(((unsigned char) value) & 0b00001111) >> 0];
	}
	else
	{
		out << std::hex << std::setw(sizeof(int_t)) << std::setfill('0') << value;
	}
}

template<typename int_t>
inline void print_bin(std::ostream& out, int_t value)
{
	out << std::bitset<sizeof(int_t) * 8>(value);
}

inline void print_bin_star(std::ostream& out, unsigned char value, unsigned char stars)
{
	for (unsigned char bit = 0b10000000; bit != 0; bit >>= 1)
	{
		out << ((stars & bit) != 0 ? '-' : ((value & bit) != 0 ? '1' : '0'));
	}
}

inline void unprint_bin_star(std::istream& in, unsigned char& value, unsigned char& stars)
{
	value = 0;
	stars = 0;
	char c;
	for (unsigned char bit = 0b10000000; bit != 0; bit >>= 1)
	{
		in.get(c);
		(c == '-' ? stars : (c == '1' ? value : bit)) |= bit;
	}
}

void print_sbox(std::ostream& out, const byte* sbox);

void print_table(std::ostream& out, const DDT_t& table);

void print_table(std::ostream& out, const terms_t& table);
void unprint_table(std::istream& in, terms_t& table);

void print_chart(std::ostream& out, const chart_t& chart);
void unprint_chart(std::istream& in, chart_t& chart);
