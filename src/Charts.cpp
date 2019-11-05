
#include "Charts.h"

#include <algorithm>

void fill_chart(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart)
{
	unsigned int mtsize = static_cast<unsigned int>(minterms.size());
	unsigned int pisize = static_cast<unsigned int>(primeimplicants.size());

	mintermschart.resize(mtsize);
	primeimplicantschart.resize(pisize);

	const auto* itpi = primeimplicants.data();
	for (unsigned int ipi = 0; ipi < pisize; itpi++, ipi++)
	{
		auto pi = (*itpi);
		const auto* itmt = minterms.data();
		for (unsigned int imt = 0; imt < mtsize; itmt++, imt++)
		{
			if (minterm_match((*itmt), pi))
			{
				mintermschart[imt].push_back(ipi);
				primeimplicantschart[ipi].push_back(imt);
			}
		}
	}
}

void primeimplicants_reduce_good(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result)
{
	unsigned int mtsize = static_cast<unsigned int>(minterms.size());
	unsigned int pisize = static_cast<unsigned int>(primeimplicants.size());

	auto chart_remove_mt = [&] (int imt)
	{
		auto& mtpi = mintermschart[imt];
		for (int ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (int ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (int imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
	};

	for (unsigned int imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		unsigned int max = 0;
		unsigned int pos = 0;

		const auto* it = primeimplicantschart.data();
		for (unsigned int i = 0; i < pisize; it++, i++)
		{
			if ((*it).size() > max)
			{
				max = static_cast<unsigned int>((*it).size());
				pos = i;
			}
		}

		if (max == 0) break;

		chart_remove_pi(pos);
	}
}

void primeimplicants_reduce_well(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result)
{
	unsigned int mtsize = static_cast<unsigned int>(minterms.size());
	unsigned int pisize = static_cast<unsigned int>(primeimplicants.size());

	auto chart_remove_mt = [&] (int imt)
	{
		auto& mtpi = mintermschart[imt];
		for (int ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (int ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (int imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
	};

	for (unsigned int imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		unsigned int max = 0;
		unsigned int min = 0;
		unsigned int pos = 0;

		const auto* it = primeimplicantschart.data();
		for (unsigned int i = 0; i < pisize; it++, i++)
		{
			if ((*it).size() >= max)
			{
				unsigned int cur = 0;
				for (int imt : (*it))
				{
					cur += static_cast<unsigned int>(mintermschart[imt].size());
				}
				if ((*it).size() > max || cur < min)
				{
					max = static_cast<unsigned int>((*it).size());
					min = cur;
					pos = i;
				}
			}
		}

		if (max == 0) break;

		chart_remove_pi(pos);
	}
}

void primeimplicants_reduce_hope(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result)
{
	std::vector<float> piscore(primeimplicants.size());

	auto score_update_pi = [&] (int ipi)
	{
		float score = 0.0;

		auto& pimt = primeimplicantschart[ipi];
		for (int imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			score += static_cast<float>(1.0 / mtpi.size());
		}

		piscore[ipi] = score;
	};

	unsigned int mtsize = static_cast<unsigned int>(minterms.size());
	unsigned int pisize = static_cast<unsigned int>(primeimplicants.size());

	auto chart_remove_mt = [&] (int imt)
	{
		auto& mtpi = mintermschart[imt];
		for (int ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
			score_update_pi(ipi);
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (int ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (int imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_remove_mt(imt);
		}
		pimt.clear();
		piscore[ipi] = 0.0;
	};

	for (unsigned int ipi = 0; ipi < pisize; ipi++)
	{
		float score = 0.0;

		auto& pimt = primeimplicantschart[ipi];
		for (int imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			score += static_cast<float>(1.0 / mtpi.size());
		}

		piscore[ipi] = score;
	}

	for (unsigned int imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_remove_pi(ipi);
		}
	}

	for(;;)
	{
		float score = 0.0;
		unsigned int min = 0;
		unsigned int pos = 0;

		const auto* it = primeimplicantschart.data();
		const auto* its = piscore.data();
		for (unsigned int i = 0; i < pisize; its++, it++, i++)
		{
			if ((*its) >= score)
			{
				unsigned int cur = static_cast<unsigned int>((*it).size());
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
