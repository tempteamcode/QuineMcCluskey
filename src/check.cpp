
#include <fstream>

#include <set>

#include "LUTs.h"
#include "types.h"

#include "SBoxes.h"
#include "Tables.h"
#include "Charts.h"

#include "print.h"

bool check(const minterms_t& minterms, const terms_t& primeimplicants)
{
	std::set<two_bytes> original;

	original.insert(minterms.cbegin(), minterms.end());


	std::set<two_bytes> implied;

	for (two_two_bytes primeimplicant : primeimplicants)
	{
		auto& pi_val = primeimplicant.two_bytes_1.bytes;
		auto& pi_mask = primeimplicant.two_bytes_2.bytes;

		if (pi_mask == 0)
		{
			implied.emplace(primeimplicant.two_bytes_1);
		}
		else
		{
			two_bytes_bytes bitmask[16]; //+1
			int bitscount = 0;

			for (two_bytes_bytes singlemask = 1; singlemask != 0; singlemask <<= 1)
			{
				if ((singlemask & pi_mask) != 0)
					bitmask[bitscount++] = singlemask;
			}
			//bitmask[bitscount] = 0;

			two_bytes_bytes curmask = 0;
			int pos = 0;

			for (;;)
			{
				two_bytes term; term.bytes = pi_val | curmask;
				implied.emplace(term);
				if (curmask == pi_mask) break;

				for (int pos = 0; ; pos++)
				{
					curmask ^= bitmask[pos];
					if ((curmask & bitmask[pos]) != 0) break;
				}

				/*
				int pos = 0;
				while ((curmask & bitmask[pos]) != 0)
				{
					curmask ^= bitmask[pos];
					pos++;
				}
				curmask ^= bitmask[pos];
				if (pos == bitscount) break;
				*/
			}
		}
	}


	return (original == implied);
}

int main_new()
{
	LUTs_init();


	std::cout << "Computing S-box..." << std::endl;

	byte SBox[256];
	fill_SBox_AES(SBox);

	std::cout << "Result:" << std::endl;
	print_sbox(std::cout, SBox);
	std::cout << std::endl << std::endl;


	std::cout << "Computing DDT..." << std::endl;

	DDT_t DDT = {};
	fill_DDT(SBox, DDT);


	std::cout << "Computing Quine-McCluskey..." << std::endl;

	minterms_t minterms;
	termsmap_t termsmap;
	DDT_extract_minterms(DDT, 0, true, minterms);


	std::cout << "Checking reduction..." << std::endl;

	terms_t primeimplicants_reduced;

	{
		std::ifstream fin_pireduced("primeimplicants_reduced_hope.txt");
		if (!fin_pireduced) return 1;
		unprint_table(fin_pireduced, primeimplicants_reduced);

		bool correct = check(minterms, primeimplicants_reduced);
		if (correct)
		{
			std::cout << "Correct." << std::endl;
		}
		else
		{
			std::cerr << "Mistake!" << std::endl;
		}
	}

	return 0;
}
