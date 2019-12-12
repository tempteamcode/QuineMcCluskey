#pragma once

#include <bitset>
#include <iostream>

#include "terms.h"

template <int size>
using PetricksTuple = std::bitset<size>;

template <typename type>
void primeimplicants_reduce_step_essential(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void PetricksMethod(const minterms_t<type>& minterms_full, const terms_t<type>& primeimplicants_full, chart_t<type>& mintermschart_full, chart_t<type>& primeimplicantschart_full, terms_t<type>& result);

template <typename type>
void PetricksMethod(const minterms_t<type>& minterms_full, const terms_t<type>& primeimplicants_full, chart_t<type>& mintermschart_full, chart_t<type>& primeimplicantschart_full, terms_t<type>& result)
{
	primeimplicants_reduce_step_essential<type>(minterms_full, primeimplicants_full, mintermschart_full, primeimplicantschart_full, result);
	// primeimplicants_reduce_step_remove<type>(minterms_full, primeimplicants_full, mintermschart_full, primeimplicantschart_full);
	minterms_t<type> minterms;
	terms_t<type> primeimplicants;
	chart_t<type> mintermschart;
	chart_t<type> primeimplicantschart;
	fill_chart_reduced<type>(minterms_full, primeimplicants_full, mintermschart_full, primeimplicantschart_full, minterms, primeimplicants, mintermschart, primeimplicantschart);

	using big = type::big;

	big upperbound;
	{
		terms_t<type> result_upperbound;
		primeimplicants_reduce_hope<type>(minterms_full, primeimplicants_full, mintermschart_full, primeimplicantschart_full, result_upperbound);
		upperbound = static_cast<big>(result_upperbound.size() - result.size());
	}

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	if (pisize > 128) throw std::overflow_error("");
	typedef PetricksTuple<128> piset;

	std::vector<piset> pisets;
	std::vector<piset> pisets_prev;

	//piset pis_empty;
	//pisets.push_back(pis_empty);
	pisets.emplace_back();

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

		if (pis_new.count() <= upperbound)
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

		std::cout << ' ' << imt << '/' << mtsize << " - " << pisets.size() << " candidate subsets" << std::endl;
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
