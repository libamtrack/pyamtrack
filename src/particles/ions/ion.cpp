#include "ion.h"

#include <stdexcept>

#include "../particles.h"

Ion::Ion(long long pdg_code) {
  nb::object p;
  try {
    p = from_pdg(pdg_code);
  } catch (const std::exception& e) {
    throw std::invalid_argument("Invalid PDG code: " + std::to_string(pdg_code));
  }
  try {
    *this = nb::cast<Ion>(p);
  } catch (const nb::cast_error&) {
    throw std::invalid_argument("Use particles.Particle() constructor for PDG codes corresponding to non-ions");
  }
}

Ion::Ion() : Particle() {
  Z = -1;
  A = -1;
  element_acronym = "unknown";
  element_name = "unknown";
  atomic_weight = 0.0;
  pdg = 0;
}