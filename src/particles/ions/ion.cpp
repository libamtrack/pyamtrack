#include "ion.h"
#include "../construct_utils.h"
#include "../particles.h"

extern "C" {
#include "AT_DataParticle.h"
}

Ion::Ion(const std::string& isotope) : Particle(isotope) {
  const auto& data = AT_Particle_Data;
  auto [symbol, A_] = parse_isotope(isotope);

  if (symbol == "proton") {
    symbol = "H";
    A_ = 1;
  } else if (symbol == "alpha") {
    symbol = "He";
    A_ = 2;
  }

  int it = -1;
  for (int i = 0; i < data.n; ++i) {
    if (to_lower_case(data.element_acronym[i]) == to_lower_case(symbol)) {
      it = i;
      break;
    }
    if (to_lower_case(data.element_name[i]) == to_lower_case(symbol)) {
      it = i;
      break;
    }
  }

  if (it != -1) {
    Z = data.Z[it];
    A = A_;
    pdg = std::make_optional<long long>(calculatePDG(Z, A));
  }
}
