#pragma once

#include <vector>
#include <array>
#include <unordered_map>

#include "types.h"

typedef bignum table_t[byte_values][byte_values];

void fill_table(byte* sbox, table_t& table);

typedef std::unordered_map<two_two_bytes, bool> subtablecell_t;
typedef std::vector<subtablecell_t> subtable_t;
typedef std::array<subtable_t, byte_bits * 2 + 1> subtables_t;

typedef std::vector<two_two_bytes> tablestar_t;

void fill_subtables(table_t& table, int value, bool keep, subtables_t& subtables);
void table_extract(table_t& table, int value, bool keep, subtable_t& subtable);
void QuineMcCluskey(subtables_t& subtables);

void fill_table(const subtables_t& subtables, tablestar_t& table);

void table_filter(subtable_t& minterms, tablestar_t& table);
