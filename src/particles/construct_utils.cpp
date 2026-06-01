#include <map>
#include <string>
#include <stdexcept>
#include <regex>

#include "construct_utils.h"

#include <iostream>

std::pair<std::string, int> parse_isotope(std::string isotope) {
  // carbon, c, C, C12, C-12, caRbOn_12, ...
  std::regex pattern(R"(([A-Za-z]+)[_-]?([1-9][0-9]*)?)");

  // 12C, 12-C, 12_C, 12Carbon, 12cArbon, ...
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

// uint
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

// Does not modify original data, just creates auxiliary maps for validation and default A values. This way we preserve data integrity while facilitating particle construction based on symbol and optional mass number.
std::string to_lower_case(const std::string& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return result;
}

