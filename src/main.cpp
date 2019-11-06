
#include <fstream>

#include "LUTs.h"
#include "print.h"

#include "DDTs.h"
#include "SBoxes.h"
#include "QuineMcCluskey.h"
#include "Heuristics.h"

#include "types.h"

int main()
{
	init_LUTs();
	init_SBoxes();

	std::cout << "Computing S-box..." << std::endl;

	auto& SBox = SBox_AES;

	std::cout << "Result:" << std::endl;
	print_sbox<with8bits>(std::cout, SBox);
	std::cout << std::endl << std::endl;


	std::cout << "Computing DDT..." << std::endl;

	DDT_t<with8bits> DDT = {};

	{
	std::ifstream fin_DDT("DDT.txt");
	if (fin_DDT) {
		fill_DDT<with8bits>(SBox, DDT);
	}
	else {
		std::ofstream fout_DDT("DDT.txt");
		if (!fout_DDT) return 1;
		fill_DDT<with8bits>(SBox, DDT);
		print_table<with8bits>(fout_DDT, DDT);

//		std::cout << "Result:" << std::endl;
//		print_table<with8bits>(std::cout, DDT);
//		std::cout << std::endl << std::endl;
	}
	}


	std::cout << "Computing Quine-McCluskey..." << std::endl;

	minterms_t<with16bits> minterms;
	termsmap_t<with16bits> termsmap;
	DDT_extract_minterms<with8bits, with16bits>(DDT, 0, true, minterms);

	terms_t<with16bits> primeimplicants;

	{
	std::ifstream fin_pi("primeimplicants.txt");
	if (fin_pi) {
		unprint_table<with16bits>(fin_pi, primeimplicants);
	}
	else {
		std::ofstream fout_pi("primeimplicants.txt");
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
	std::ifstream fin_chart("chart.txt");
	if (fin_chart) {
		unprint_chart<with16bits>(fin_chart, mintermschart);
		unprint_chart<with16bits>(fin_chart, primeimplicantschart);
	}
	else {
		std::ofstream fout_chart("chart.txt");
		if (!fout_chart) return 1;
		fill_chart<with16bits>(minterms, primeimplicants, mintermschart, primeimplicantschart);
		print_chart<with16bits>(fout_chart, mintermschart);
		print_chart<with16bits>(fout_chart, primeimplicantschart);
	}
	}

	/*
	std::cout << "Computing fast reduction..." << std::endl;

	{
		std::ofstream fout_pifast("primeimplicants_reduced_fast.txt");
		if (!fout_pifast) return 1;
		primeimplicants_reduce_fast<with16bits>(minterms, primeimplicants);
		print_table<with16bits>(fout_pifast, primeimplicants);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants);
//		std::cout << std::endl << std::endl;
	}
	//*/
	/*
	std::cout << "Computing good reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_good;

	{
		std::ofstream fout_pigood("primeimplicants_reduced_good.txt");
		if (!fout_pigood) return 1;
		primeimplicants_reduce_good<with16bits>(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_good);
		print_table<with16bits>(fout_pigood, primeimplicants_good);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_good);
//		std::cout << std::endl << std::endl;
	}
	//*/
	/*
	std::cout << "Computing better reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_well;

	{
		std::ofstream fout_piwell("primeimplicants_reduced_well.txt");
		if (!fout_piwell) return 1;
		primeimplicants_reduce_well<with16bits>(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_well);
		print_table<with16bits>(fout_piwell, primeimplicants_well);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_well);
//		std::cout << std::endl << std::endl;
	}
	//*/

	std::cout << "Computing hope reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_hope;

	{
		std::ofstream fout_pihope("primeimplicants_reduced_hope.txt");
		if (!fout_pihope) return 1;
		primeimplicants_reduce_hope<with16bits>(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_hope);
		print_table<with16bits>(fout_pihope, primeimplicants_hope);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_hope);
//		std::cout << std::endl << std::endl;
	}


	{
	std::cout << "Checking reduction..." << std::endl;

	terms_t<with16bits> primeimplicants_reduced;

	{
		std::ifstream fin_pireduced("primeimplicants_reduced_hope.txt");
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
