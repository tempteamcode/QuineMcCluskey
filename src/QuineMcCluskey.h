#pragma once

#include <vector>
#include <array>

#include "terms.h"

using bool_byte = unsigned char;
static const bool_byte bool_byte_true = -1;
static const bool_byte bool_byte_false = 0;

template <typename type>
using termsmapcell_t = std::pair<terms_t<type>,std::vector<bool_byte>>;
template <typename type>
using termsmapcol_t = std::vector<termsmapcell_t<type>>;
template <typename type>
using termsmap_t = std::array<termsmapcol_t<type>, type::bits_nb * 2 + 1>;

template <typename type>
void QuineMcCluskey(const minterms_t<type>& minterms, termsmap_t<type>& termsmap);
template <typename type>
void termsmap_extract_primeimplicants(const termsmap_t<type>& termsmap, terms_t<type>& primeimplicants);

template <typename type>
using chart_t = std::vector<std::vector<typename type::big>>;

template <typename type>
void fill_chart(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart);

template <typename type>
void fill_chart_reduced(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, const chart_t<type>& mintermschart, const chart_t<type>& primeimplicantschart, minterms_t<type>& minterms_reduced, terms_t<type>& primeimplicants_reduced, chart_t<type>& mintermschart_reduced, chart_t<type>& primeimplicantschart_reduced);

template <typename type>
void primeimplicants_reduce_step_unique(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);

template <typename type>
void primeimplicants_reduce_step_essential(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result);


#include <unordered_set>

#include "LUTs.h"

template <typename type>
void QuineMcCluskey(const minterms_t<type>& minterms, termsmap_t<type>& termsmap)
{
	using big = typename type::big;

	using byte = typename type::byte;
	using pair = typename type::pair;

	termsmapcol_t<type>& mintermscol = termsmap[0];
	mintermscol.resize(type::bits_nb * 2 + 1);

	pair tuple;

	tuple.second = 0;
	for (const byte& minterm : minterms)
	{
		unsigned char row = bitscount(minterm);

		tuple.first = minterm;
		mintermscol[row].first.push_back(tuple);
	}
	for (termsmapcell_t<type>& mintermsgroup : mintermscol)
	{
		mintermsgroup.second.resize(mintermsgroup.first.size(), bool_byte_true);
	}


	pair diffs;
	pair tupleprod;

	for (unsigned int col = 0; col + 1 < termsmap.size(); col++)
	{
		termsmapcol_t<type>& subtablecurr = termsmap[col];
		termsmapcol_t<type>& subtablenext = termsmap[col + 1];
		subtablenext.resize(subtablecurr.size() - 1);

		for (unsigned int row = 0; row + 1 < subtablecurr.size(); row++)
		{
			termsmapcell_t<type>& cellcurr = subtablecurr[row];
			termsmapcell_t<type>& cellnext = subtablecurr[row + 1];
			termsmapcell_t<type>& cellprod = subtablenext[row];

			big icurrsize = static_cast<big>(cellcurr.first.size());
			big inextsize = static_cast<big>(cellnext.first.size());


			if (inextsize > type::bits_nb * 100)
			{
				if (col == 0)
				{
					std::sort(cellnext.first.begin(), cellnext.first.end());
				}

				const pair* itcurr = cellcurr.first.data();
				for (big icurr = 0; icurr < icurrsize; itcurr++, icurr++)
				{
					const pair& itemcurr = *itcurr;

					pair cellcomp[type::bits_nb];
					big icompsize = 0;

					{
						pair itemcomp = itemcurr;
						const byte bits = type::byte_mask ^ itemcurr.second;
						for (byte bit = 1; bit != 0; bit <<= 1)
						{
							if ((bit & bits) != 0)
							{
								itemcomp.first = itemcurr.first ^ bit;
								if ((itemcomp.first & bit) != 0)
									cellcomp[icompsize++] = itemcomp;
							}
						}
					}

					if (icompsize > 1) std::sort(cellcomp, cellcomp + icompsize);

					const auto itnextbeg = cellnext.first.begin();
					const auto itnextend = cellnext.first.end();

					auto itnextcur = itnextbeg;
					for (big icomp = 0; icomp < icompsize; icomp++)
					{
						pair itemcomp = cellcomp[icomp];

						itnextcur = std::lower_bound(itnextcur, itnextend, itemcomp);
						if (itnextcur == itnextend) break;

						if (*itnextcur == itemcomp)
						{
							const pair& itemnext = *itnextcur;
							big inext = static_cast<big>(itnextcur - itnextbeg);

							tupleprod.first = (itemcurr.first & itemnext.first);
							tupleprod.second = ((itemcurr.second | itemnext.second) | (itemcurr.first ^ itemnext.first));

							auto pos = std::lower_bound(cellprod.first.begin(), cellprod.first.end(), tupleprod);
							if (pos == cellprod.first.end() || (!((*pos) == tupleprod)))
							{
								cellprod.first.insert(pos, tupleprod);
							}

							cellcurr.second[icurr] = bool_byte_false;
							cellnext.second[inext] = bool_byte_false;
						}
					}

				}

			}
			else
			{

				const pair* itcurr = cellcurr.first.data();
				for (big icurr = 0; icurr < icurrsize; itcurr++, icurr++)
				{
					const pair& itemcurr = *itcurr;
					const pair* itnext = cellnext.first.data();
					for (big inext = 0; inext < inextsize; itnext++, inext++)
					{
						const pair& itemnext = *itnext;
						diffs.bytes = itemcurr.bytes ^ itemnext.bytes;
						if (bitscount(diffs.first) + bitscount(diffs.second) == 1)
						{
							tupleprod.first = (itemcurr.first & itemnext.first);
							tupleprod.second = ((itemcurr.second | itemnext.second) | diffs.first);

							auto pos = std::lower_bound(cellprod.first.begin(), cellprod.first.end(), tupleprod);
							if (pos == cellprod.first.end() || (!((*pos) == tupleprod)))
							{
								cellprod.first.insert(pos, tupleprod);
							}

							cellcurr.second[icurr] = bool_byte_false;
							cellnext.second[inext] = bool_byte_false;
						}
					}
				}

			}
		}

		for (termsmapcell_t<type>& subtablecell : subtablenext)
		{
			subtablecell.second.resize(subtablecell.first.size(), bool_byte_true);
		}
	}
}

template <typename type>
void termsmap_extract_primeimplicants(const termsmap_t<type>& termsmap, terms_t<type>& primeimplicants)
{
	using big = typename type::big;

	for (const termsmapcol_t<type>& termsmapcol : termsmap)
	{
		for (const termsmapcell_t<type>& implicants : termsmapcol)
		{
			big isize = static_cast<big>(implicants.first.size());
			const auto* it = implicants.second.data();
			for (big i = 0; i < isize; it++, i++)
			{
				if (*it != bool_byte_false)
				{
					primeimplicants.push_back(implicants.first[i]);
				}
			}
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
void fill_chart_reduced(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, const chart_t<type>& mintermschart, const chart_t<type>& primeimplicantschart, minterms_t<type>& minterms_reduced, terms_t<type>& primeimplicants_reduced, chart_t<type>& mintermschart_reduced, chart_t<type>& primeimplicantschart_reduced)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());

	minterms_reduced.clear();
	minterms_reduced.reserve(mtsize);
	for (big imt = 0; imt < mtsize; imt++)
	{
		if (!mintermschart[imt].empty())
			minterms_reduced.push_back(minterms[imt]);
	}

	big pisize = static_cast<big>(primeimplicants.size());

	primeimplicants_reduced.clear();
	primeimplicants_reduced.reserve(pisize);
	for (big ipi = 0; ipi < pisize; ipi++)
	{
		if (!primeimplicantschart[ipi].empty())
			primeimplicants_reduced.push_back(primeimplicants[ipi]);
	}

	fill_chart<type>(minterms_reduced, primeimplicants_reduced, mintermschart_reduced, primeimplicantschart_reduced);
}

template <typename type>
void primeimplicants_reduce_step_unique(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	auto chart_select_mt = [&] (big imt)
	{
		auto& mtpi = mintermschart[imt];
		for (big ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
		}
		mtpi.clear();
	};

	auto chart_select_pi = [&] (big ipi)
	{
		result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			chart_select_mt(imt);
		}
		pimt.clear();
	};

	for (big imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_select_pi(ipi);
		}
	}
}

template <typename type>
void primeimplicants_reduce_step_essential(const minterms_t<type>& minterms, const terms_t<type>& primeimplicants, chart_t<type>& mintermschart, chart_t<type>& primeimplicantschart, terms_t<type>& result)
{
	using big = typename type::big;

	big mtsize = static_cast<big>(minterms.size());
	big pisize = static_cast<big>(primeimplicants.size());

	std::unordered_set<big> piqueue;
	std::unordered_set<big> mtqueue;

	long remmt = 0;
	long rem1 = 0;
	long rempi = 0;

	auto chart_remove_mt = [&] (big imt)
	{
		auto& mtpi = mintermschart[imt];
		for (big ipi : mtpi)
		{
			auto& pimt = primeimplicantschart[ipi];
			pimt.erase(std::find(pimt.begin(), pimt.end(), imt));
			piqueue.emplace(ipi);
		}
		mtpi.clear();
	};

	auto chart_remove_pi = [&] (big ipi, bool select)
	{
		if (select) result.push_back(primeimplicants[ipi]);

		auto& pimt = primeimplicantschart[ipi];
		for (big imt : pimt)
		{
			auto& mtpi = mintermschart[imt];
			mtpi.erase(std::find(mtpi.begin(), mtpi.end(), ipi));
			if (select) chart_remove_mt(imt);
			else mtqueue.emplace(imt);
		}
		pimt.clear();
	};

	for (big imt = 0; imt < mtsize; imt++)
	{
		if (mintermschart[imt].size() == 1)
		{
			int ipi = mintermschart[imt][0];
			chart_remove_pi(ipi, true);
		}
	}

	auto chart_examine_mt = [&] (big imt) {
		std::unordered_set<big> imts;

		auto& mtpi = mintermschart[imt];
		big mtpi_size = static_cast<big>(mtpi.size());

		if (mtpi_size == 1)
		{
			chart_remove_pi(mtpi[0], true);
			rem1++;
			return;
		}

		for (big ipi : mtpi)
		{
			for (big imt_bis : primeimplicantschart[ipi])
				if (mintermschart[imt_bis].size() >= mtpi_size)
					imts.emplace(imt_bis);
		}

		for (big imt_bis : imts)
		{
			if (imt_bis == imt) continue;
			auto& mtpi_bis = mintermschart[imt_bis];
			if (std::includes(mtpi_bis.begin(), mtpi_bis.end(), mtpi.begin(), mtpi.end()))
			{
				chart_remove_mt(imt_bis);
				remmt++;
			}
		}
	};

	auto chart_examine_pi = [&] (big ipi) {
		std::unordered_set<big> ipis;

		auto& pimt = primeimplicantschart[ipi];
		big pimt_size = static_cast<big>(pimt.size());

		for (big imt : pimt)
		{
			for (big ipi_bis : mintermschart[imt])
				if (primeimplicantschart[ipi_bis].size() >= pimt_size)
					ipis.emplace(ipi_bis);
		}

		for (big ipi_bis : ipis)
		{
			if (ipi_bis == ipi) continue;
			auto& pimt_bis = primeimplicantschart[ipi_bis];
			if (std::includes(pimt_bis.begin(), pimt_bis.end(), pimt.begin(), pimt.end()))
			{
				chart_remove_pi(ipi, false);
				rempi++;
				return;
			}
		}
	};

	for (big imt = mtsize; --imt > 0; )
	{
		chart_examine_mt(imt);
	}
	piqueue.clear();

	for (big ipi = pisize; --ipi > 0; )
	{
		chart_examine_pi(ipi);
	}

	long loops = 0;

	do
	{
		std::unordered_set<big>& mtqueue_cur = mtqueue;
		std::unordered_set<big>& piqueue_cur = piqueue;
		//std::swap(mtqueue_cur, mtqueue);
		//std::swap(piqueue_cur, piqueue);

		while (!mtqueue_cur.empty())
		{
			auto itmt = mtqueue_cur.begin();
			int imt = (*itmt);
			mtqueue_cur.erase(itmt);

			chart_examine_mt(imt);
			loops++;
		}

		while (!piqueue_cur.empty())
		{
			auto itpi = piqueue_cur.begin();
			int ipi = (*itpi);
			piqueue_cur.erase(itpi);

			chart_examine_pi(ipi);
			loops++;
		}
	} while (!mtqueue.empty() || !piqueue.empty());
}
