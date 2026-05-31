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

// Opisać że nie modyfikujemy oryginalnych danych, tylko tworzymy mapy pomocnicze do walidacji i domyślnych wartości A. W ten sposób zachowujemy integralność danych, a jednocześnie ułatwiamy konstrukcję cząstek na podstawie symbolu i opcjonalnie liczby masowej.

// Does not modify original data, just creates auxiliary maps for validation and default A values. This way we preserve data integrity while facilitating particle construction based on symbol and optional mass number.
std::string to_lower_case(const std::string& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return result;
}


std::string decode_pdg_to_string(const int pdg, bool called_by_particle_constr) {
  if (abs(pdg) < 1000000000 && pdg != 2212) {
    return decode_elementary(pdg);
  }
  else {
      return decode_ion(pdg);
  }
}

std::string decode_elementary(const int pdg) {
  switch (pdg) {
    case 2112:
      return "neutron";
    case 11:
      return "electron";
    default:
      throw std::invalid_argument("Unknown particle PDG code: " + std::to_string(pdg));
  }
}

std::string decode_ion(const int pdg) {
  const auto& data = AT_Particle_Data;

  long long Z = (pdg / 10000) % 1000;
  long long A = (pdg / 10) % 1000;

  for (int i = 0; i < data.n; ++i) {
    if (data.Z[i] == Z) {
      std::string acronym(data.element_acronym[i]);
      return acronym + std::to_string(A);
    }
  }

  throw std::invalid_argument("Unknown ion PDG code: " + std::to_string(pdg));
}