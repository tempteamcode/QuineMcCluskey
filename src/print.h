#pragma once

#include <iostream>
#include <iomanip>
#include <bitset>

#include "LUTs.h"
#include "DDTs.h"
#include "terms.h"
#include "Heuristics.h"

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

template <typename type>
inline void print_bin_star(std::ostream& out, typename type::byte value, typename type::byte stars)
{
	for (typename type::byte bit = 1 << (type::bits_nb - 1); bit != 0; bit >>= 1)
	{
		out << ((stars & bit) != 0 ? '-' : ((value & bit) != 0 ? '1' : '0'));
	}
}

template <typename type>
inline void unprint_bin_star(std::istream& in, typename type::byte& value, typename type::byte& stars)
{
	value = 0;
	stars = 0;
	char c;
	for (typename type::byte bit = 1 << (type::bits_nb - 1); bit != 0; bit >>= 1)
	{
		in.get(c);
		(c == '-' ? stars : (c == '1' ? value : bit)) |= bit;
	}
}

template <typename type>
void print_sbox(std::ostream& out, const typename type::byte* sbox);

template <typename type>
void print_table(std::ostream& out, const DDT_t<type>& table);

template <typename type>
void print_table(std::ostream& out, const terms_t<type>& table);
template <typename type>
void unprint_table(std::istream& in, terms_t<type>& table);

template <typename type>
void print_chart(std::ostream& out, const chart_t<type>& chart);
template <typename type>
void unprint_chart(std::istream& in, chart_t<type>& chart);


template <typename type>
void print_sbox(std::ostream& out, const typename type::byte* sbox)
{
	using byte = typename type::byte;

	const byte* ptr = sbox;

	byte index = 0;
	byte column = 0;
	for(;;)
	{
		if (column == 0)
		{
			column = 16;
			out << '\n';
		}

		column--;
		print_hex<byte>(out << "  ", *ptr++);

		if (((++index) & type::byte_mask) == 0) break;
	}
}

template <typename type>
void print_table(std::ostream& out, const DDT_t<type>& table)
{
	using byte = typename type::byte;
	using big = typename type::big;

	byte d1 = 0;
	for(;;)
	{
		byte d2 = 0;
		for(;;)
		{
			big count = table[d1][d2];

			if (count > 0)
			{
				print_bin(out << '\n', d1);
				print_bin(out << " --> ", d2);
				out << " (" << count << '/' << type::byte_values << ')';
			}

			if (((++d2) & type::byte_mask) == 0) break;
		}

		if (((++d1) & type::byte_mask) == 0) break;
	}
}

template <typename type>
void print_terms(std::ostream& out, const terms_t<type>& terms)
{
	for (const typename type::pair& term : terms)
	{
		print_bin_star<type>(out << '\n', term.first, term.second);
	}
}

template <typename type>
void unprint_terms(std::istream& in, terms_t<type>& terms)
{
	typename type::pair term;

	char c;
	while (in.get(c)) // '\n'
	{
		unprint_bin_star<type>(in, term.first, term.second);
		terms.push_back(term);
	}
}

template <typename type>
void print_chart(std::ostream& out, const chart_t<type>& chart)
{
	out << "((" << chart.size() << "))\n";
	for (auto& row : chart)
	{
		out << '(' << row.size() << ')';
		for (int val : row) out << ' ' << val;
		out << '\n';
	}
}

template <typename type>
void unprint_chart(std::istream& in, chart_t<type>& chart)
{
	typename type::big size;
	char c;

	in.get(c).get(c); // "(("
	in >> size;
	chart.resize(size);
	in.get(c).get(c).get(c); // "))\n"

	for (auto& row : chart)
	{
		in.get(c); // '('
		in >> size;
		row.resize(size);
		in.get(c); // ')'
		for (typename type::big& val : row)
		{
			in.get(c); // ' '
			in >> val;
		}
		in.get(c); // '\n'
	}
}

template <typename typehalf>
void print_termshalf(std::ostream& out, const std::vector<std::pair<typename typehalf::pair,typename typehalf::pair>>& termshalf)
{
	for (auto& termhalfpair : termshalf)
	{
		print_bin_star<typehalf>(out, termhalfpair.first.first, termhalfpair.first.second);
		out << " --> ";
		print_bin_star<typehalf>(out, termhalfpair.second.first, termhalfpair.second.second);
		out << std::endl;
	}
}
