#pragma once
#include <string>
#include <map>

extern std::map<std::string, int> most_popular_iso_A;


extern std::pair<std::string, int> parse_isotope(std::string isotope);

long long calculatePDG(int Z, int A, int L = 0, int I = 0);