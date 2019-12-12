#pragma once

#include <vector>

template <typename type>
using minterms_t = std::vector<typename type::byte>;
template <typename type>
using terms_t = std::vector<typename type::pair>;

template <typename type>
inline bool minterm_match(const typename type::byte& lhs, const typename type::pair& rhs)
{
	return (((lhs ^ rhs.first) & (~rhs.second)) == 0);
}

template <typename type>
inline bool term_match(const typename type::pair& lhs, const typename type::pair& rhs)
{
	return
		((lhs.second & (~rhs.second)) == 0) &&
		(((lhs.first ^ rhs.first) & (~rhs.second)) == 0);
}


#include <set>

template <typename type>
bool check(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants)
{
	using byte = typename type::byte;
	using pair = typename type::pair;

	std::set<byte> original;

	original.insert(minterms.cbegin(), minterms.end());


	std::set<byte> implied;

	for (const pair& primeimplicant : primeimplicants)
	{
		const byte& pi_val = primeimplicant.first;
		const byte& pi_mask = primeimplicant.second;

		for (byte pi_bits = pi_mask; pi_bits > 0; pi_bits = (pi_mask & (pi_bits - 1)))
		{
			implied.emplace(pi_val | pi_bits);
		}
		implied.emplace(pi_val);
	}


	return (original == implied);
}
