#include <map>
#include <string>
#include <stdexcept>
#include <regex>

#include "construct_utils.h"

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
  std::regex pattern(R"(([A-Z][a-z]*)(_?[1-9]+[0-9]*)?)");

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
