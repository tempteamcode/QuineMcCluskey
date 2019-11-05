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

void print_table(std::ostream& out, const DDT_t& table)
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
				out << " (" << table[d1][d2] << "/256)";
			}

			if (((++d2) & byte_mask) == 0) break;
		}

		if (((++d1) & byte_mask) == 0) break;
	}
}

void print_table(std::ostream& out, const terms_t& table)
{
	for (const two_two_bytes& tuple : table)
	{
		print_bin_star(out << '\n', tuple.two_bytes_1.byte_1, tuple.two_bytes_2.byte_1);
		print_bin_star(out << " --> ", tuple.two_bytes_1.byte_2, tuple.two_bytes_2.byte_2);
	}
}

void unprint_table(std::istream& in, terms_t& table)
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

void print_chart(std::ostream& out, const chart_t& chart)
{
	out << "((" << chart.size() << "))\n";
	for (auto& row : chart)
	{
		out << '(' << row.size() << ')';
		for (int val : row) out << ' ' << val;
		out << '\n';
	}
}

void unprint_chart(std::istream& in, chart_t& chart)
{
	unsigned int size;
	char c;

	in.get(c).get(c); // "(("
	in >> size;
	chart.resize(size);
	in.get(c).get(c).get(c); // "))\n"

	for (auto& row : chart)
	{
		in.get(c); // '('
		in >> size;
		row.resize(size);
		in.get(c); // ')'
		for (int& val : row)
		{
			in.get(c); // ' '
			in >> val;
		}
		in.get(c); // '\n'
	}
}
