
#include <fstream>

#include "LUTs.h"
#include "print.h"

#include "DDTs.h"
#include "SBoxes.h"
#include "QuineMcCluskey.h"
#include "Heuristics.h"

#include "types.h"


struct {
	int sbox_index;
	bool keep;
	int value;
	int heuristic_index;
} arguments;

static const int count_SBoxes = 5;
static const char * const names_SBoxes[count_SBoxes] = {"AES", "M0", "M1", "M2", "M3"};
typedef const uint8_t (*sbox_ptr)[256];
static const sbox_ptr array_SBoxes[count_SBoxes] = { &SBox_AES, &SBox_Midori128_S0, &SBox_Midori128_S1, &SBox_Midori128_S2, &SBox_Midori128_S3 };
static const int count_heuristics = 4;
static const char * const names_heuristics[count_heuristics] = {"fastest", "maxmt", "maxmtminpi", "mostlikely"};
typedef void (*heuristic_ptr) (const minterms_t<with16bits>&, const terms_t<with16bits>&, chart_t<with16bits>&, chart_t<with16bits>&, terms_t<with16bits>&);
static const heuristic_ptr array_heuristics[count_heuristics] = { &primeimplicants_reduce_good<with16bits>, &primeimplicants_reduce_good<with16bits>, &primeimplicants_reduce_well<with16bits>, &primeimplicants_reduce_hope<with16bits> };

bool str_eq(const char* str, const char* str2)
{
	for(;;)
	{
		char c = *str++;
		if (c != *str2++) return false;
		if (c == '\0') return true;
	}
}

bool parseInt(const char* str, int& value)
{
	value = 0;
	for(;;)
	{
		char c = *str++;

		if (c == '\0') return true;

		int digit = c - '0';
		if (digit < 0 || digit > 9) return false;

		value = (value * 10) + digit;
	}
}

bool parseArgs(const char* name, const char* choice, const char* heuristic)
{
	arguments.sbox_index = count_SBoxes;
	for (int index = 0; index < count_SBoxes; index++) {
		if (str_eq(name, names_SBoxes[index])) arguments.sbox_index = index;
	}
	if (arguments.sbox_index == count_SBoxes) return false;

	if (*choice == '=')
		arguments.keep = true;
	else if (*choice == '!')
		arguments.keep = false;
	else
		return false;

	if (!parseInt(++choice, arguments.value))
		return false;

	arguments.heuristic_index = count_heuristics;
	for (int index = 0; index < count_heuristics; index++) {
		if (str_eq(heuristic, names_heuristics[index])) arguments.heuristic_index = index;
	}
	if (arguments.heuristic_index == count_heuristics) return false;

	return true;
}

int main(int argc, const char* argv[])
{
	init_LUTs();
	init_SBoxes();

	const char * name_value;
	if (argc <= 1)
		parseArgs(names_SBoxes[0], name_value = "!0", names_heuristics[count_heuristics - 1]);
	else if ((argc != 4) || (!parseArgs(argv[1], name_value = argv[2], argv[3])))
	{
		std::cerr << "Incorrect command line arguments." << std::endl;
		return 1;
	}

	std::string prefix = names_SBoxes[arguments.sbox_index];

	const sbox_ptr SBox_ptr = array_SBoxes[arguments.sbox_index];
	const uint8_t * SBox = &(*SBox_ptr)[0];

	std::cout << "S-box:" << std::endl;
	print_sbox<with8bits>(std::cout, SBox);
	std::cout << std::endl << std::endl;


	std::cout << "Computing DDT..." << std::endl;

	DDT_t<with8bits> DDT = {};

	{
	std::ifstream fin_DDT(prefix + "_DDT.txt");
	fill_DDT<with8bits>(SBox, DDT);
	if (!fin_DDT) {
		std::ofstream fout_DDT(prefix + "_DDT.txt");
		if (!fout_DDT) return 1;
		print_table<with8bits>(fout_DDT, DDT);

//		std::cout << "Result:" << std::endl;
//		print_table<with8bits>(std::cout, DDT);
//		std::cout << std::endl << std::endl;
	}
	}


	prefix = prefix + name_value + "_";

	minterms_t<with16bits> minterms;
	termsmap_t<with16bits> termsmap;
	DDT_extract_minterms<with8bits, with16bits>(DDT, arguments.value, arguments.keep, minterms);

	std::cout << "Computing Quine-McCluskey..." << std::endl;

	terms_t<with16bits> primeimplicants;

	{
	std::ifstream fin_pi(prefix + "primeimplicants.txt");
	if (fin_pi) {
		unprint_table<with16bits>(fin_pi, primeimplicants);
	}
	else {
		std::ofstream fout_pi(prefix + "primeimplicants.txt");
		if (!fout_pi) return 1;

		QuineMcCluskey<with16bits>(minterms, termsmap);
		termsmap_extract_primeimplicants<with16bits>(termsmap, primeimplicants);

		print_table<with16bits>(fout_pi, primeimplicants);

//		std::cout << "Result:" << std::endl;
//		print_table<with16bits>(std::cout, primeimplicants);
//		std::cout << std::endl << std::endl;
	}
	}


	std::cout << "Computing prime implicants chart..." << std::endl;

	chart_t<with16bits> mintermschart;
	chart_t<with16bits> primeimplicantschart;

	{
	std::ifstream fin_chart(prefix + "chart.txt");
	if (fin_chart) {
		unprint_chart<with16bits>(fin_chart, mintermschart);
		unprint_chart<with16bits>(fin_chart, primeimplicantschart);
	}
	else {
		std::ofstream fout_chart(prefix + "chart.txt");
		if (!fout_chart) return 1;
		fill_chart<with16bits>(minterms, primeimplicants, mintermschart, primeimplicantschart);
		print_chart<with16bits>(fout_chart, mintermschart);
		print_chart<with16bits>(fout_chart, primeimplicantschart);
	}
	}


	std::cout << "Computing " << names_heuristics[arguments.heuristic_index] << " reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_reduced;

	{
		std::ofstream fout_pireduced(prefix + "primeimplicants_reduced_" + names_heuristics[arguments.heuristic_index] + ".txt");
		if (!fout_pireduced) return 1;
		if (arguments.heuristic_index == 0)
		{
			minterms_t<with16bits> minterms_copy = minterms;
			primeimplicants_reduced = primeimplicants;
			primeimplicants_reduce_fast<with16bits>(minterms_copy, primeimplicants_reduced);
		}
		else
		{
			(*array_heuristics[arguments.heuristic_index])(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_reduced);
		}
		print_table<with16bits>(fout_pireduced, primeimplicants_reduced);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_reduced);
//		std::cout << std::endl << std::endl;
	}


	{
	std::cout << "Checking reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_reduced;

	{
		std::ifstream fin_pireduced(prefix + "primeimplicants_reduced_" + names_heuristics[arguments.heuristic_index] + ".txt");
		if (!fin_pireduced) return 1;
		unprint_table<with16bits>(fin_pireduced, primeimplicants_reduced);

		bool correct = check<with16bits>(minterms, primeimplicants_reduced);
		if (correct)
		{
			std::cout << "Correct." << std::endl;
		}
		else
		{
			std::cerr << "Mistake!" << std::endl;
		}
	}
	}


	std::cout << "Done." << std::endl;

	return 0;
}
