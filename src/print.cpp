#include <iostream>

#include "print.h"
#include "LUTs.h"

void print_sbox(std::ostream& out, const byte* sbox)
{
	const byte* ptr = sbox;
	bignum count = byte_values;

	int line = 0;
	while (count > 0)
	{
		if (line == 0)
		{
			line = 16;
			out << '\n';
		}

		print_hex<byte>(out << "  ", (count--, line--, *ptr++));
	}
}

void print_table(std::ostream& out, const table_t& table)
{
	byte d1 = 0;
	for (;;)
	{
		byte d2 = 0;
		for (;;)
		{
			bignum count = table[d1][d2];

			if (count > 0)
			{
				print_bin(out << '\n', d1);
				print_bin(out << " --> ", d2);
				std::cout << " (" << table[d1][d2] << "/256)";
			}

			if (((++d2) & byte_mask) == 0) break;
		}

		if (((++d1) & byte_mask) == 0) break;
	}
}

void print_table(std::ostream& out, const tablestar_t& table)
{
    for (const two_two_bytes& tuple : table)
    {
        print_bin_star(out << '\n', tuple.two_bytes_1.byte_1, tuple.two_bytes_2.byte_1);
        print_bin_star(out << " --> ", tuple.two_bytes_1.byte_2, tuple.two_bytes_2.byte_2);
    }
}

void unprint_table(std::istream& in, tablestar_t& table)
{
    two_two_bytes tuple;
    char c;
    while (in.get(c)) // '\n'
    {
        unprint_bin_star(in, tuple.two_bytes_1.byte_1, tuple.two_bytes_2.byte_1);
        in.get(c).get(c).get(c).get(c).get(c); // " --> "
        unprint_bin_star(in, tuple.two_bytes_1.byte_2, tuple.two_bytes_2.byte_2);
        table.push_back(tuple);
    }
}
