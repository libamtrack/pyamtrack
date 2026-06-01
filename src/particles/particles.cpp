#include "particles.h"
#include "construct_utils.h"
#include "ions/ion.h"
#include "ions/ions.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <stdexcept>

#include <iostream>


Particle::Particle() {
  element_acronym = "unknown";
  element_name = "unknown";
  atomic_weight = 0.0;
  id = -1;
  pdg = 0;
}

Particle::Particle(long long pdg_code) {
  nb::object p;
  try {
    p = from_pdg(pdg_code);
  } catch (const std::exception& e) {
    throw std::invalid_argument("Invalid PDG code: " + std::to_string(pdg_code));
  }
  try {
    *this = nb::cast<Ion>(p);
    throw std::invalid_argument("Use particles.ions.Ion() constructor for PDG codes corresponding to ions");
  } catch (const nb::cast_error&) {
    *this = nb::cast<Particle>(p);
  }
}


/**
 * @brief Constructs a Particle from a string representation.
 *
 * The input string can represent:
 *   - An element acronym only (e.g. "He", "C", "U"), or
 *   - An isotope notation with a leading mass number followed by the acronym
 *     (e.g. "3He", "12C", "238U").
 *
 * Parsing logic:
 *   1. Scan the string from the beginning to extract any leading digits
 *      (interpreted as the mass number A).
 *   2. The remainder of the string is treated as the element acronym.
 *   3. Create a Particle using the acronym.
 *   4. If a mass number was found, set it as the Particle's A.
 *
 * @param name The string representation of the particle.
 * @return Particle The constructed Particle object.
 * @throws std::invalid_argument If the string is malformed
 *         (e.g. empty acronym, unknown acronym).
 */

nb::object from_string(const std::string& name) {

  // logic for creating either Particle or Ion. Should be changed with further development of the class hierarchy. For now, we just check for special cases of proton, neutron and electron, which are not really ions, but we want to support them as special particles.

  if (name == "neutron" || name == "electron") {
    return nb::cast(create_particle(name));
  }
  
  return nb::cast(create_ion(name));
}

// helper functions for creating particles and ions. The point is to isolate the logic of choosing to create either the Particle or the Ion from construction
Particle create_particle(const std::string& name) {
  Particle p = Particle();
  p.element_name = name;
  p.element_acronym = name[0];
  p.pdg = (name == "neutron") ? 2112 : 11;
  
  return p;
}
 
Ion create_ion(const std::string& name) {
  Ion p = Ion();
  const auto& data = AT_Particle_Data;
  auto [symbol, A_] = parse_isotope(name);

  std::string acronym;
  std::string element_name;

  if (symbol == "proton") {
    symbol = "H";
    A_ = 1;
  } else if (symbol == "alpha") {
    symbol = "He";
    A_ = 4;
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

  if (it == -1) {
    throw nb::value_error(("Unknown element name or acronym: " + symbol).c_str());
  }

  acronym = data.element_acronym[it];

  auto [l, h] = isotope_A_range[acronym];

  if (A_ != -1) {
    if (A_ < l || A_ > h) {
      throw nb::value_error(("Invalid mass number A=" + std::to_string(A_) + " for element " + acronym + " (valid range: " + std::to_string(l) + "-" + std::to_string(h) + ")").c_str());
    }
  } else if (A_ == -1) {
    A_ = most_popular_iso_A[acronym];
  }
  p.id = it + 1;
  p.atomic_weight = data.atomic_weight[it];
  p.element_name = std::string(data.element_name[it]);
  p.element_acronym = acronym;
  p.Z = data.Z[it];
  p.A = A_;
  
  
  return p;
}


Ion from_ZA(long long Z, long long A) {
  const auto& data = AT_Particle_Data;
  for (int i = 0; i < data.n; ++i) {
    if (data.Z[i] == Z) {
      std::string acronym(data.element_acronym[i]);
      nb::object result = from_string(acronym + std::to_string(A));
      try {
        Ion ion = nb::cast<Ion>(result);
        return ion;
      } catch (const nb::cast_error&) {
        throw std::invalid_argument(
          "Expected Ion type for Z=" + std::to_string(Z) + 
          ", A=" + std::to_string(A) + 
          ", but got Particle instead"
        );
      }
    }
  }

  throw std::invalid_argument("Ion with Z=" + std::to_string(Z) + " not found");
}

nb::object from_pdg(long long pdg_code) {
  if (pdg_code == 2212) {
    return nb::cast(from_ZA(1, 1));
  }
  
  if (pdg_code == 2112) {
    return nb::cast(from_string("neutron"));
  }
  if (pdg_code == 11) {
    return nb::cast(from_string("electron"));
  }

  const auto& data = AT_Particle_Data;


  if (pdg_code < 1000000000) {
    throw nb::value_error(("PDG code does not correspond to a nucleus: " + std::to_string(pdg_code)).c_str());
  }
  if (pdg_code > 11000000000) {
    throw nb::value_error(("PDG code is too large to be a valid nucleus: " + std::to_string(pdg_code)).c_str());
  }

  long long Z = (pdg_code / 10000) % 1000;
  long long A = (pdg_code / 10) % 1000;

  return nb::cast(from_ZA(Z, A));
}

std::vector<std::string> get_names() {
  std::vector<std::string> names;
  names.reserve(AT_Particle_Data.n);
  for (int i = 0; i < AT_Particle_Data.n; ++i) {
    names.emplace_back(AT_Particle_Data.element_name[i]);
  }
  return names;
}

std::vector<std::string> get_acronyms() {
  std::vector<std::string> acronyms;
  acronyms.reserve(AT_Particle_Data.n);
  for (int i = 0; i < AT_Particle_Data.n; ++i) {
    acronyms.emplace_back(AT_Particle_Data.element_acronym[i]);
  }
  return acronyms;
}


std::string Particle::str() const {
    return element_acronym;  
}

std::string Particle::repr() const {
    return "Particle(name=\"" + element_name +
           "\", acronym=\"" + element_acronym + "\")";
}