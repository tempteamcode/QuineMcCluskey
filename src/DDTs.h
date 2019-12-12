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
