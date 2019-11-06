#pragma once

#include "terms.h"

template <typename type>
void primeimplicants_reduce_fast(minterms_t<type>& minterms, terms_t<type>& primeimplicants);

template <typename type>
using chart_t = std::vector<std::vector<typename type::big>>;

template <typename type>
void fill_chart(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart);

template <typename type>
void primeimplicants_reduce_good(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void primeimplicants_reduce_well(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void primeimplicants_reduce_hope(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);


#include <algorithm>

/* #include "print.h" */

template <typename type> /* ,typename typehalf */
void primeimplicants_reduce_fast(minterms_t<type>& minterms, terms_t<type>& primeimplicants)
{
	using pair = typename type::pair;

	for (auto itr = primeimplicants.cend(); itr != primeimplicants.cbegin();)
	{
		--itr;
		const pair r = *itr;

		bool useful = false;

		for (auto itl = minterms.cbegin(); itl != minterms.cend(); ++itl)
		{
			if (minterm_match<type>((*itl), r))
			{
				/*
				print_bin_star<type>(out << '\n', itl.first, itl.second);
				print_bin_star<type>(out << " <== ", itr.first, itr.second);
				*/
				useful = true;

				itl = minterms.erase(itl);
				if (itl == minterms.cend()) break;
			}
		}

		if (!useful)
		{
			itr = primeimplicants.erase(itr);
		}
	}
}


template <typename type>
void fill_chart(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	mintermschart.resize(mtsize);
	primeimplicantschart.resize(pisize);

	const auto* itpi = primeimplicants.data();
	for (big ipi = 0; ipi < pisize; itpi++, ipi++)
	{
		auto pi = (*itpi);
		const auto* itmt = minterms.data();
		for (big imt = 0; imt < mtsize; itmt++, imt++)
		{
			if (minterm_match<type>((*itmt), pi))
			{
				mintermschart[imt].push_back(ipi);
				primeimplicantschart[ipi].push_back(imt);
			}
		}
	}
}

template <typename type>
void primeimplicants_reduce_good(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	auto chart_remove_mt = [&] (big imt)
	{
		auto& mtpi = mintermschart[imt];
		for (big ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (big ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
	};

	for (big imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		big max = 0;
		big pos = 0;

		const auto* it = primeimplicantschart.data();
		for (big i = 0; i < pisize; it++, i++)
		{
			if ((*it).size() > max)
			{
				max = static_cast<big>((*it).size());
				pos = i;
			}
		}

		if (max == 0) break;

		chart_remove_pi(pos);
	}
}

template <typename type>
void primeimplicants_reduce_well(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	auto chart_remove_mt = [&] (big imt)
	{
		auto& mtpi = mintermschart[imt];
		for (big ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (big ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
	};

	for (big imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			big ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		big max = 0;
		big min = 0;
		big pos = 0;

		const auto* it = primeimplicantschart.data();
		for (big i = 0; i < pisize; it++, i++)
		{
			if ((*it).size() >= max)
			{
				big cur = 0;
				for (big imt : (*it))
				{
					cur += static_cast<big>(mintermschart[imt].size());
				}
				if ((*it).size() > max || cur < min)
				{
					max = static_cast<big>((*it).size());
					min = cur;
					pos = i;
				}
			}
		}

		if (max == 0) break;

		chart_remove_pi(pos);
	}
}

template <typename type>
void primeimplicants_reduce_hope(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	using big = type::big;

	std::vector<float> piscore(primeimplicants.size());

	auto score_update_pi = [&] (big ipi)
	{
		float score = 0.0;

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			score += static_cast<float>(1.0 / mtpi.size());
		}

		piscore[ipi] = score;
	};

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	auto chart_remove_mt = [&] (big imt)
	{
		auto& mtpi = mintermschart[imt];
		for (big ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
			score_update_pi(ipi);
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (big ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
		piscore[ipi] = 0.0;
	};

	for (big ipi = 0; ipi < pisize; ipi++)
	{
		float score = 0.0;

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			score += static_cast<float>(1.0 / mtpi.size());
		}

		piscore[ipi] = score;
	}

	for (big imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			big ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		float score = 0.0;
		big min = 0;
		big pos = 0;

		const auto* it = primeimplicantschart.data();
		const auto* its = piscore.data();
		for (big i = 0; i < pisize; its++, it++, i++)
		{
			if ((*its) >= score)
			{
				big cur = static_cast<big>((*it).size());
				if ((*its) > score || cur < min)
				{
					score = (*its);
					min = cur;
					pos = i;
				}
			}
		}

		if (min == 0) break;

		chart_remove_pi(pos);
	}
}
