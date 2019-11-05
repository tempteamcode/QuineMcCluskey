
#include <algorithm>

#include "Tables.h"

#include "LUTs.h"

void fill_table(byte* sbox, table_t& table)
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

void fill_table(const subtables_t& subtables, tablestar_t& table)
{
    for (const subtable_t& subtable : subtables)
    {
        for (const subtablecell_t& subtablecell : subtable)
        {
            for (auto it = subtablecell.begin(); it != subtablecell.end(); ++it)
            {
                if ((*it).second)
                {
                    table.push_back((*it).first);
                }
            }
        }
    }
}

void fill_subtables(table_t& table, int value, bool keep, subtables_t& subtables)
{
	subtable_t& subtable = subtables[0];

	table_extract(table, value, keep, subtable);

	QuineMcCluskey(subtables);
}

void table_extract(table_t& table, int value, bool keep, subtable_t& subtable)
{
	subtable.resize(byte_bits * 2 + 1);

	two_two_bytes tuple;
	tuple.two_bytes_2.bytes = 0;

	byte d1 = 0;
	for (;;)
	{
		byte d2 = 0;
		for (;;)
		{
			bignum count = table[d1][d2];

			if ((count == value) == keep)
			{
				tuple.two_bytes_1.byte_1 = d1;
				tuple.two_bytes_1.byte_2 = d2;

				unsigned char bitscount = LUT_bitscount_256[d1] + LUT_bitscount_256[d2];
				subtable[bitscount][tuple] = true;
			}

			if (((++d2) & byte_mask) == 0) break;
		}

		if (((++d1) & byte_mask) == 0) break;
	}
}

void QuineMcCluskey(subtables_t& subtables)
{
	for (unsigned int col = 0; col + 1 < subtables.size(); col++)
	{
		subtable_t& subtablecurr = subtables[col];
		subtable_t& subtablenext = subtables[col + 1];
		subtablenext.resize(subtablecurr.size() - 1);

		for (unsigned int row = 0; row+1 < subtablecurr.size(); row++)
		{
			subtablecell_t& cellcurr = subtablecurr[row];
			subtablecell_t& cellnext = subtablecurr[row + 1];
			subtablecell_t& cellprod = subtablenext[row];

            for (auto itcurr = cellcurr.begin(); itcurr != cellcurr.end(); ++itcurr)
            {
                const two_two_bytes& itemcurr = (*itcurr).first;
                for (auto itnext = cellnext.begin(); itnext != cellnext.end(); ++itnext)
                {
                    const two_two_bytes& itemnext = (*itnext).first;

					two_bytes diffany; diffany.bytes = (itemcurr.two_bytes_2.bytes ^ itemnext.two_bytes_2.bytes);
					two_bytes diffbit; diffbit.bytes = (itemcurr.two_bytes_1.bytes ^ itemnext.two_bytes_1.bytes);// &~(itemcurr.two_bytes_2.bytes | itemnext.two_bytes_2.bytes);
					if (LUT_bitscount_256[diffbit.byte_1] + LUT_bitscount_256[diffbit.byte_2] + LUT_bitscount_256[diffany.byte_1] + LUT_bitscount_256[diffany.byte_2] == 1)
					{
						two_two_bytes tupleprod;
						tupleprod.two_bytes_1.bytes = (itemcurr.two_bytes_1.bytes & itemnext.two_bytes_1.bytes);
						tupleprod.two_bytes_2.bytes = (itemcurr.two_bytes_2.bytes | itemnext.two_bytes_2.bytes) | diffbit.bytes;

						cellprod[tupleprod] = true;

                        (*itcurr).second = false;
                        (*itnext).second = false;
					}
                }
            }
		}
	}
}

#include "print.h"

void table_filter(subtable_t& minterms, tablestar_t& table)
{
//  for (subtablecell_t cell : minterms)
//  {
//      for (auto& it : cell)
//      {
//          it.second = true;
//      }
//  }

    auto match = [] (const two_two_bytes& lhs, const two_two_bytes& rhs)
    {
        return
            ((lhs.two_bytes_2.bytes & (~rhs.two_bytes_2.bytes)) == 0) &&
            (((lhs.two_bytes_1.bytes ^ rhs.two_bytes_1.bytes) & (~rhs.two_bytes_2.bytes)) == 0);
    };

    for (auto itr = table.cend(); itr != table.cbegin();)
    {
        --itr;

        bool useful = false;

        for (subtablecell_t& cell : minterms)
        {
            for (auto itl = cell.cbegin(); itl != cell.cend(); ++itl)
            {
//              if ((*itl).second)
//              {
                    if (match((*itl).first, (*itr)))
                    {
                        print_bin_star(std::cout << '\n', itl->first.two_bytes_1.byte_1, itl->first.two_bytes_2.byte_1);
                        print_bin_star(std::cout << " --> ", itl->first.two_bytes_1.byte_2, itl->first.two_bytes_2.byte_2);
                        print_bin_star(std::cout << " <== ", itr->two_bytes_1.byte_1, itr->two_bytes_2.byte_1);
                        print_bin_star(std::cout << " --> ", itr->two_bytes_1.byte_2, itr->two_bytes_2.byte_2);
                        useful = true;

                        itl = cell.erase(itl);
                        if (itl == cell.cend()) break;
                    }
//              }
            }
        }
        
        if (!useful)
        {
            itr = table.erase(itr);
        }
    }
}
