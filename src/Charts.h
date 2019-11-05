#pragma once

#include "Tables.h"

typedef std::vector<std::vector<int>> chart_t;

void fill_chart(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart);

void primeimplicants_reduce_good(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result);

void primeimplicants_reduce_well(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result);

void primeimplicants_reduce_hope(const minterms_t& minterms, const terms_t& primeimplicants, chart_t& mintermschart, chart_t& primeimplicantschart, terms_t& result);
