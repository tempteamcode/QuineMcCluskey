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
