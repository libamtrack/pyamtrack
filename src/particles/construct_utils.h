#pragma once
#include <string>
#include <map>



extern std::pair<std::string, int> parse_isotope(std::string isotope);

long long calculatePDG(int Z, int A, int L = 0, int I = 0);

std::string to_lower_case(const std::string& s);

inline std::map<std::string, int> most_popular_iso_A = {
	{"H", 1},    // protium
	{"He", 4},
	{"Li", 7},
	{"Be", 9},
	{"B", 11},
	{"C", 12},
	{"N", 14},
	{"O", 16},
	{"F", 19},
	{"Ne", 20},

	{"Na", 23},
	{"Mg", 24},
	{"Al", 27},
	{"Si", 28},
	{"P", 31},
	{"S", 32},
	{"Cl", 35},  // actually mostly 35 and 37; 35 is more abundant
	{"Ar", 40},

	{"K", 39},
	{"Ca", 40},
	{"Sc", 45},
	{"Ti", 48},
	{"V", 51},
	{"Cr", 52},
	{"Mn", 55},
	{"Fe", 56},
	{"Co", 59},
	{"Ni", 58},
	{"Cu", 63},
	{"Zn", 64},

	{"Br", 79},
	{"Kr", 84},

	{"Ag", 107},
	{"I", 127},
	{"Xe", 132},

	{"Ba", 138},
	{"Au", 197},
	{"Hg", 202},
	{"Pb", 208}
};