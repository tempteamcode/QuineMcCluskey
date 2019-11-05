
#include <algorithm>

#include "Tables.h"

#include "LUTs.h"

void fill_DDT(byte* sbox, DDT_t& table)
{
	byte i1 = 0;
	for (;;)
	{
		byte o1 = sbox[i1];

		byte i2 = 0;
		for (;;)
		{
			byte o2 = sbox[i2];

			table[i1 ^ i2][o1 ^ o2]++;

			if (((++i2) & byte_mask) == 0) break;
		}

		if (((++i1) & byte_mask) == 0) break;
	}
}

void DDT_extract_minterms(const DDT_t& table, int value, bool keep, minterms_t& minterms)
{
	two_bytes tuple;
	byte& d1 = tuple.byte_1;
	byte& d2 = tuple.byte_2;

	d1 = 0;
	for (;;)
	{
		d2 = 0;
		for (;;)
		{
			bignum count = table[d1][d2];

			if ((count == value) == keep)
			{
				minterms.push_back(tuple);
			}

			if (((++d2) & byte_mask) == 0) break;
		}

		if (((++d1) & byte_mask) == 0) break;
	}
}

void QuineMcCluskey(const minterms_t& minterms, termsmap_t& termsmap)
{
	termsmapcol_t& mintermscol = termsmap[0];
	mintermscol.resize(byte_bits * 2 + 1);

	two_two_bytes tuple;

	tuple.two_bytes_2.bytes = 0;
	for (const two_bytes& minterm : minterms)
	{
		unsigned char bitscount = LUT_bitscount_256[minterm.byte_1] + LUT_bitscount_256[minterm.byte_2];

		tuple.two_bytes_1.bytes = minterm.bytes;
		mintermscol[bitscount].first.push_back(tuple);
	}
	for (termsmapcell_t& mintermsgroup : mintermscol)
	{
		mintermsgroup.second.resize(mintermsgroup.first.size(), 0b1);
	}


	two_two_bytes diffs;
	two_two_bytes tupleprod;

	for (unsigned int col = 0; col + 1 < termsmap.size(); col++)
	{
		termsmapcol_t& subtablecurr = termsmap[col];
		termsmapcol_t& subtablenext = termsmap[col + 1];
		subtablenext.resize(subtablecurr.size() - 1);

		for (unsigned int row = 0; row + 1 < subtablecurr.size(); row++)
		{
			termsmapcell_t& cellcurr = subtablecurr[row];
			termsmapcell_t& cellnext = subtablecurr[row + 1];
			termsmapcell_t& cellprod = subtablenext[row];

			unsigned int icurrsize = static_cast<unsigned int>(cellcurr.first.size());
			unsigned int inextsize = static_cast<unsigned int>(cellnext.first.size());

			const two_two_bytes* itcurr = cellcurr.first.data();
			for (unsigned int icurr = 0; icurr < icurrsize; itcurr++, icurr++)
			{
				const two_two_bytes& itemcurr = *itcurr;
				const two_two_bytes* itnext = cellnext.first.data();
				for (unsigned int inext = 0; inext < inextsize; itnext++, inext++)
				{
					const two_two_bytes& itemnext = *itnext;
					diffs.bytes = itemcurr.bytes ^ itemnext.bytes;
					if (LUT_bitscount_256[diffs.two_bytes_1.byte_1] + LUT_bitscount_256[diffs.two_bytes_1.byte_2] + LUT_bitscount_256[diffs.two_bytes_2.byte_1] + LUT_bitscount_256[diffs.two_bytes_2.byte_2] == 1)
					{
						tupleprod.two_bytes_1.bytes = (itemcurr.two_bytes_1.bytes & itemnext.two_bytes_1.bytes);
						tupleprod.two_bytes_2.bytes = ((itemcurr.two_bytes_2.bytes | itemnext.two_bytes_2.bytes) | diffs.two_bytes_1.bytes);

						auto pos = std::lower_bound(cellprod.first.begin(), cellprod.first.end(), tupleprod);
						if (pos == cellprod.first.end() || (!((*pos) == tupleprod)))
						{
							cellprod.first.insert(pos, tupleprod);
						}

						cellcurr.second[icurr] = 0b0;
						cellnext.second[inext] = 0b0;
					}
				}
			}
		}

		for (termsmapcell_t& subtablecell : subtablenext)
		{
			subtablecell.second.resize(subtablecell.first.size(), 0b1);
		}
	}
}

void termsmap_extract_primeimplicants(const termsmap_t& termsmap, terms_t& primeimplicants)
{
	for (const termsmapcol_t& termsmapcol : termsmap)
	{
		for (const termsmapcell_t& implicants : termsmapcol)
		{
			unsigned int isize = static_cast<unsigned int>(implicants.first.size());
			const auto* it = implicants.second.data();
			for (unsigned int i = 0; i < isize; it++, i++)
			{
				if (*it != 0b0)
				{
					primeimplicants.push_back(implicants.first[i]);
				}
			}
		}
	}
}

/*
#include "print.h"
*/

void primeimplicants_reduce_fast(minterms_t& minterms, terms_t& primeimplicants)
{
	for (auto itr = primeimplicants.cend(); itr != primeimplicants.cbegin();)
	{
		--itr;
		const two_two_bytes r = *itr;

		bool useful = false;

		for (auto itl = minterms.cbegin(); itl != minterms.cend(); ++itl)
		{
			if (minterm_match((*itl), r))
			{
				/*
				print_bin_star(std::cout << '\n', itl->two_bytes_1.byte_1, itl->two_bytes_2.byte_1);
				print_bin_star(std::cout << " --> ", itl->two_bytes_1.byte_2, itl->two_bytes_2.byte_2);
				print_bin_star(std::cout << " <== ", itr->two_bytes_1.byte_1, itr->two_bytes_2.byte_1);
				print_bin_star(std::cout << " --> ", itr->two_bytes_1.byte_2, itr->two_bytes_2.byte_2);
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
