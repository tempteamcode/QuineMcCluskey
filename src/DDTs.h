#pragma once

#include "terms.h"

template <typename type>
using DDT_t = typename type::big[type::byte_values][type::byte_values];

template <typename type>
void fill_DDT(const typename type::byte* sbox, DDT_t<type>& table);

template <typename type, typename typepair>
void DDT_extract_minterms(const DDT_t<type>& table, typename type::big value, bool keep, minterms_t<typepair>& minterms);


template <typename type>
void fill_DDT(const typename type::byte* sbox, DDT_t<type>& table)
{
	using byte = typename type::byte;

	byte i1 = 0;
	for(;;)
	{
		byte o1 = sbox[i1];

		byte i2 = 0;
		for(;;)
		{
			byte o2 = sbox[i2];

			table[i1 ^ i2][o1 ^ o2]++;

			if (((++i2) & type::byte_mask) == 0) break;
		}

		if (((++i1) & type::byte_mask) == 0) break;
	}
}

template <typename type, typename typepair>
void DDT_extract_minterms(const DDT_t<type>& table, typename type::big value, bool keep, minterms_t<typepair>& minterms)
{
	using byte = typename type::byte;
	using pair = typename type::pair;
	using big = typename type::big;

	pair tuple;
	byte& d1 = tuple.first;
	byte& d2 = tuple.second;

	d1 = 0;
	for(;;)
	{
		d2 = 0;
		for(;;)
		{
			big count = table[d1][d2];

			if ((count == value) == keep)
			{
				minterms.push_back(tuple.bytes);
			}

			if (((++d2) & type::byte_mask) == 0) break;
		}

		if (((++d1) & type::byte_mask) == 0) break;
	}
}

template <typename type, typename typehalf>
inline void term_split(const typename type::pair& term, std::pair<typename typehalf::pair, typename typehalf::pair>& result)
{
	typename typehalf::pair p1;
	typename typehalf::pair p2;

	p1.bytes = term.first;
	p2.bytes = term.second;

	{
		typename typehalf::byte tmp;
		tmp = p1.second;
		p1.second = p2.first;
		p2.first = tmp;
	}

	result.first = p1;
	result.second = p2;
}

template <typename type, typename typehalf>
void terms_split(const terms_t<type>& terms, std::vector<std::pair<typename typehalf::pair,typename typehalf::pair>>& result)
{
	size_t count = terms.size();

	result.resize(count);
	for (int i = 0; i < count; i++)
	{
		term_split<type,typehalf>(terms[i], result[i]);
	}
}
