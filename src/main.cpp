
#include <fstream>

#include "LUTs.h"
#include "types.h"

#include "SBoxes.h"
#include "Tables.h"
#include "Charts.h"

#include "print.h"

int main()
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

	{
	std::ifstream fin_DDT("DDT.txt");
	if (fin_DDT) {
		fill_DDT(SBox, DDT);
	}
	else {
		std::ofstream fout_DDT("DDT.txt");
		if (!fout_DDT) return 1;
		fill_DDT(SBox, DDT);
		print_table(fout_DDT, DDT);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, DDT);
//		std::cout << std::endl << std::endl;
	}
	}


	std::cout << "Computing Quine-McCluskey..." << std::endl;

	minterms_t minterms;
	termsmap_t termsmap;
	DDT_extract_minterms(DDT, 0, true, minterms);

	terms_t primeimplicants;

	{
	std::ifstream fin_pi("primeimplicants.txt");
	if (fin_pi) {
		unprint_table(fin_pi, primeimplicants);
	}
	else {
		std::ofstream fout_pi("primeimplicants.txt");
		if (!fout_pi) return 1;

		QuineMcCluskey(minterms, termsmap);
		termsmap_extract_primeimplicants(termsmap, primeimplicants);

		print_table(fout_pi, primeimplicants);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants);
//		std::cout << std::endl << std::endl;
	}
	}


	std::cout << "Computing prime implicants chart..." << std::endl;

	chart_t mintermschart;
	chart_t primeimplicantschart;

	{
	std::ifstream fin_chart("chart.txt");
	if (fin_chart) {
		unprint_chart(fin_chart, mintermschart);
		unprint_chart(fin_chart, primeimplicantschart);
	}
	else {
		std::ofstream fout_chart("chart.txt");
		if (!fout_chart) return 1;
		fill_chart(minterms, primeimplicants, mintermschart, primeimplicantschart);
		print_chart(fout_chart, mintermschart);
		print_chart(fout_chart, primeimplicantschart);
	}
	}

	/*
	std::cout << "Computing good reduction..." << std::endl;

	terms_t primeimplicants_good;

	{
		std::ofstream fout_pigood("primeimplicants_reduced_good.txt");
		if (!fout_pigood) return 1;
		primeimplicants_reduce_good(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_good);
		print_table(fout_pigood, primeimplicants_good);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_good);
//		std::cout << std::endl << std::endl;
	}
	*/
	/*
	std::cout << "Computing better reduction..." << std::endl;

	terms_t primeimplicants_well;

	{
		std::ofstream fout_piwell("primeimplicants_reduced_well.txt");
		if (!fout_piwell) return 1;
		primeimplicants_reduce_well(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_well);
		print_table(fout_piwell, primeimplicants_well);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_well);
//		std::cout << std::endl << std::endl;
	}
	*/

	std::cout << "Computing hope reduction..." << std::endl;

	terms_t primeimplicants_hope;

	{
		std::ofstream fout_pihope("primeimplicants_reduced_hope.txt");
		if (!fout_pihope) return 1;
		primeimplicants_reduce_hope(minterms, primeimplicants, mintermschart, primeimplicantschart, primeimplicants_hope);
		print_table(fout_pihope, primeimplicants_hope);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants_hope);
//		std::cout << std::endl << std::endl;
	}

	/*
	std::cout << "Computing fast reduction..." << std::endl;

	{
		std::ofstream fout_pifast("primeimplicants_reduced_fast.txt");
		if (!fout_pifast) return 1;
		primeimplicants_reduce_fast(minterms, primeimplicants);
		print_table(fout_pifast, primeimplicants);

//		std::cout << "Result:" << std::endl;
//		print_table(std::cout, primeimplicants);
//		std::cout << std::endl << std::endl;
	}
	*/

	std::cout << "Done." << std::endl;

	return 0;
}
