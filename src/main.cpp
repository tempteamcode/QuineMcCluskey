
#include <fstream>

#include "LUTs.h"
#include "types.h"

#include "Tables.h"
#include "SBoxes.h"

#include "print.h"

int main()
{
	LUTs_init();

	byte SBox_AES[256];
	fill_SBox_AES(SBox_AES);

	std::cout << "SBox from AES:" << std::endl;
	print_sbox(std::cout, SBox_AES);
	std::cout << std::endl << std::endl;

	table_t Table_AES = {};
	fill_table(SBox_AES, Table_AES);

//	std::cout << "Table from AES:" << std::endl;
//	print_table(std::cout, Table_AES);
//	std::cout << std::endl << std::endl;

	subtables_t subTables_AES;
    table_extract(Table_AES, 0, true, subTables_AES[0]);

    tablestar_t TableStar_AES;
    
    std::ifstream fin("file.txt");
    if (fin) {
        unprint_table(fin, TableStar_AES);
    }
    else {
        std::ofstream fout("file.txt");
        if (!fout) return 1;

        QuineMcCluskey(subTables_AES);
        fill_table(subTables_AES, TableStar_AES);

        print_table(fout, TableStar_AES);
        print_table(std::cout, TableStar_AES);
    }

    std::ofstream fout_filt("file_filtered.txt");
    if (!fout_filt) return 1;
    table_filter(subTables_AES[0], TableStar_AES);
    print_table(fout_filt, TableStar_AES);

    return 0;
}
