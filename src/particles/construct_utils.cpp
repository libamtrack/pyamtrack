#include <map>
#include <string>
#include <stdexcept>
#include <regex>

#include "construct_utils.h"

#include <iostream>

// std::map<std::string, int> pdg_to_symbol = {
//   {"gamma", 1},
//   {"e-", 2},
//   {"e+", 3},
//   {"mu-", 4},
//   {"mu+", 5},
//   {"pi0", 6},
//   {"pi-", 7},
//   {"pi+", 8},
//   {"K0L", 9},
//   {"K0S", 10},
//   {"K-", 11},
//   {"K+", 12},
//   {"p", 13},
//   {"n", 14},
//   {"nu_e", 15},
//   {"anti_nu_e", 16},
//   {"nu_mu", 17},
//   {"anti_nu_mu", 18},
  
// };



std::pair<std::string, int> parse_isotope(std::string isotope) {
  std::regex pattern(R"(([A-Za-z]+)[_-]?([1-9][0-9]*)?)");

  std::regex pattern_alternative(R"(([1-9][0-9]*)[_-]?([A-Za-z]+))");

  std::smatch match;

  std::smatch match2;

  int A;
  std::string symbol;

  if (!std::regex_match(isotope, match, pattern)) {
    if (!std::regex_match(isotope, match2, pattern_alternative)) {
      throw std::invalid_argument("Invalid isotope format: " + isotope);
    } else{
      // std::cout << match2[1].str() << " " << match2[2].str() << "\n";
      A = std::stoi(match2[1].str());
      symbol = match2[2];
      return {symbol, A};
    }
  } else {
    symbol = match[1];
    if (match[2] == "") {
      return {symbol, -1};
    } else {
      A = std::stoi(match[2].str());
      return {symbol, A};
    }

  }


  
}

long long calculatePDG(int Z, int A, int L, int I) {
  if (Z == A == 1) {
    return 2212;
  }
  return 1000000000LL
    + L * 10000000LL
    + Z * 10000LL
    + A * 10LL
    + I;
}

std::string to_lower_case(const std::string& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return result;
}
