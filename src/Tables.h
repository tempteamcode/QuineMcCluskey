#pragma once

#include <vector>
#include <array>

#include "types.h"

typedef bignum DDT_t[byte_values][byte_values];

void fill_DDT(byte* sbox, DDT_t& table);

typedef std::vector<two_bytes> minterms_t;
typedef std::vector<two_two_bytes> terms_t;

typedef std::pair<terms_t,std::vector<uint8_t>> termsmapcell_t;
typedef std::vector<termsmapcell_t> termsmapcol_t;
typedef std::array<termsmapcol_t, byte_bits * 2 + 1> termsmap_t;

void DDT_extract_minterms(const DDT_t& table, int value, bool keep, minterms_t& minterms);
void QuineMcCluskey(const minterms_t& minterms, termsmap_t& termsmap);
void termsmap_extract_primeimplicants(const termsmap_t& termsmap, terms_t& primeimplicants);

void primeimplicants_reduce_fast(minterms_t& minterms, terms_t& primeimplicants);

inline bool minterm_match(const two_bytes& lhs, const two_two_bytes& rhs)
{
	return (((lhs.bytes ^ rhs.two_bytes_1.bytes) & (~rhs.two_bytes_2.bytes)) == 0);
}

inline bool term_match(const two_two_bytes& lhs, const two_two_bytes& rhs)
{
	return
		((lhs.two_bytes_2.bytes & (~rhs.two_bytes_2.bytes)) == 0) &&
		(((lhs.two_bytes_1.bytes ^ rhs.two_bytes_1.bytes) & (~rhs.two_bytes_2.bytes)) == 0);
}
