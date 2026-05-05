#include <map>
#include <string>
#include <stdexcept>
#include <regex>

#include "construct_utils.h"

std::map<std::string, int> pdg_to_symbol = {
  {"gamma", 1},
  {"e-", 2},
  {"e+", 3},
  {"mu-", 4},
  {"mu+", 5},
  {"pi0", 6},
  {"pi-", 7},
  {"pi+", 8},
  {"K0L", 9},
  {"K0S", 10},
  {"K-", 11},
  {"K+", 12},
  {"p", 13},
  {"n", 14},
  {"nu_e", 15},
  {"anti_nu_e", 16},
  {"nu_mu", 17},
  {"anti_nu_mu", 18},
  
};

std::map<std::string, int> most_popular_iso_A = {
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


std::pair<std::string, int> parse_isotope(std::string isotope) {
  std::regex pattern(R"(([A-Z][a-z]?|[a-z]+)(_[1-9]+[0-9]*)?)");

  std::smatch match;

  if (!std::regex_match(isotope, match, pattern)) {
    throw std::invalid_argument("Invalid particle format: " + isotope);
  }


  std::string symbol = match[1];
  if (match[2] == "") {
    return {symbol, -1};
  } else {
    int A = std::stoi(match[2].str().substr(1));
    return {symbol, A};
  }
}

// long long calculatePDG(int Z, int A, int L = 0, int I = 0) {
//   if (Z == A == 1) {
//     return 2212;
//   }
//   return 1000000000LL
//     + L * 10000000LL
//     + Z * 10000LL
//     + A * 10LL
//     + I;
// }