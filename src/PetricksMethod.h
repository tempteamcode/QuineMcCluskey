#pragma once

#include <bitset>

#include "terms.h"

template <int size>
using PetricksTuple = std::bitset<size>;

template <typename type>
void primeimplicants_reduce_step_essential(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void PetricksMethod(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void PetricksMethod(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	// primeimplicants_reduce_hope<type>(...);
	// big lower_bound = result.size();
	// result.clear();

	primeimplicants_reduce_step_essential<type>(minterms, primeimplicants, mintermschart, primeimplicantschart, result);
	// primeimplicants_reduce_step_remove<type>(minterms, primeimplicants, mintermschart, primeimplicantschart);

	using big = type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	if (pisize > 256) throw std::overflow_error("");
	typedef PetricksTuple<256> piset;

	std::vector<piset> pisets;
	std::vector<piset> pisets_prev;

	{
		piset pis_empty;
		pisets.push_back(pis_empty);
	}

	auto pisets_add = [&] (const piset& pis_new)
	{
		for (auto it = pisets.begin(); it != pisets.end(); )
		{
			const piset& pis_old = (*it);

			if (pis_old == pis_new) return;
			piset pis_and = pis_old & pis_new;
			if (pis_and == pis_old) return;
			if (pis_and == pis_new)
			{
				it = pisets.erase(it); // pisets.remove(it);
			}
			else
			{
				++it;
			}
		}

		// if (pis_new.count() <= lower_bound)
		pisets.push_back(pis_new);
	};

	for (big imt = 0; imt < mtsize; imt++)
	{
		const auto& mtpi = mintermschart[imt];
		if (mtpi.size() == 0) continue;

		pisets_prev.clear();
		std::swap(pisets, pisets_prev);

		piset pis_mt;
		for (big ipi : mtpi)
		{
			pis_mt.set(ipi);
		}

		for (const piset& pis_cur : pisets_prev)
		{
			if ((pis_cur & pis_mt).any())
			{
				pisets_add(pis_cur);
			}
			else
			{
				piset pis_res = pis_cur;
				for (big ipi : mtpi)
				{
					pis_res.set(ipi);
					pisets_add(pis_res);
					pis_res.set(ipi, false);
				}
			}
		}
	}

	big min = pisize + 1;
	size_t pos = 0;

	size_t pisets_count = pisets.size();
	for (size_t index = 0; index < pisets_count; index++)
	{
		if (pisets[index].count() < min)
		{
			min = static_cast<big>(pisets[index].count());
			pos = index;
		}
	}

	if (min < pisize + 1)
	{
		piset pis_min = pisets[pos];

		for (big ipi = 0; ipi < pisize; ipi++)
		{
			if (pis_min.test(ipi))
			{
				result.push_back(primeimplicants[ipi]);
			}
		}
	}
}
