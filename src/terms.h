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
	using pairbytes = decltype(type::pair::bytes);

	std::set<byte> original;

	original.insert(minterms.cbegin(), minterms.end());


	std::set<byte> implied;

	for (typename type::pair primeimplicant : primeimplicants)
	{
		auto& pi_val = primeimplicant.first;
		auto& pi_mask = primeimplicant.second;

		if (pi_mask == 0)
		{
			implied.emplace(primeimplicant.first);
		}
		else
		{
			pairbytes bitmask[type::bits_nb];

			int pos = 0;
			for (pairbytes singlemask = 1; singlemask != 0; singlemask <<= 1)
			{
				if ((singlemask & pi_mask) != 0)
					bitmask[pos++] = singlemask;
			}

			pairbytes curmask = 0;

			for (;;)
			{
				implied.emplace(pi_val | curmask);
				if (curmask == pi_mask) break;

				for (int pos = 0; ; pos++)
				{
					curmask ^= bitmask[pos];
					if ((curmask & bitmask[pos]) != 0) break;
				}
			}
		}
	}


	return (original == implied);
}
