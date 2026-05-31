#include "particles.h"
#include "construct_utils.h"
#include "ions/ion.h"
#include "ions/ions.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <stdexcept>

#include <iostream>

// Particle::Particle(string isotope) {
//   if (id < 1 || id > AT_Particle_Data.n) {
//     throw std::invalid_argument("Invalid particle id: " + std::to_string(id));
//   }
//   size_t index = static_cast<size_t>(id - 1);
//   Z = AT_Particle_Data.Z[index];
//   atomic_weight = AT_Particle_Data.atomic_weight[index];
//   element_name = std::string(AT_Particle_Data.element_name[index]);
//   element_acronym = std::string(AT_Particle_Data.element_acronym[index]);
//   density_g_cm3 = AT_Particle_Data.density_g_cm3[index];
//   I_eV_per_Z = AT_Particle_Data.I_eV_per_Z[index];
//   A = std::nullopt;
// }

Particle::Particle() {
  element_acronym = "unknown";
  element_name = "unknown";
  atomic_weight = 0.0;
  id = -1;
  pdg = 0;
}

Particle::Particle(const std::string& isotope) {
  const auto& data = AT_Particle_Data;
  auto [symbol, A_] = parse_isotope(isotope);

  std::string acronym;
  std::string element_name;

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

  if (it == -1) {
    throw nb::value_error(("Unknown element name or acronym: " + symbol).c_str());
  }

  acronym = data.element_acronym[it];
  element_name = data.element_name[it];

  auto [l, h] = isotope_A_range[acronym];

  if (A_ != -1) {
    if (A_ < l || A_ > h) {
      throw nb::value_error(("Invalid mass number A=" + std::to_string(A_) + " for element " + acronym + " (valid range: " + std::to_string(l) + "-" + std::to_string(h) + ")").c_str());
    }
  } else if (A_ == -1) {
    A_ = most_popular_iso_A[acronym];
  }
  id = it + 1;
  atomic_weight = data.atomic_weight[it];
  element_name = std::string(data.element_name[it]);
  element_acronym = acronym;

}

// Particle Particle::from_number(long particle_no) {
//   long Z_candidate = particle_no / 1000;
//   long A_candidate = AT_A_from_particle_no_single(particle_no);

//   if (A_candidate < 0) {
//     throw std::invalid_argument("Invalid particle number: " + std::to_string(particle_no));
//   }

//   const auto& data = AT_Particle_Data;
//   for (int i = 0; i < data.n; ++i) {
//     if (data.Z[i] == Z_candidate) {
//       std::string acronym(data.element_acronym[i]);
//       Particle p(acronym);
//       p.A = A_candidate;
//       return p;
//     }
//   }

//   throw std::invalid_argument("Particle with Z=" + std::to_string(Z_candidate) + " not found");
// }



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

Particle Particle::from_pdg(long long pdg_code) {
  if (pdg_code == 2212) {
    return Particle("proton");
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

  return from_ZA(Z, A);
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

long Particle::py_get_A() const {
  // Particles don't have A - this is only for Ion
  // Return a sentinel value or throw
  return -1;
}

std::string Particle::str() const {
  return "[Particle: " + element_acronym + "]";
}

std::string Particle::repr() const {
  return "[Particle " + element_name + " (id=" + std::to_string(id) + ")]";
}

const Particle Particle::proton("proton");


// nb::object create_particle(const std::string& isotope) {

//   if (isotope == "p" || isotope == "proton") {
//     return nb::cast(Particle(isotope));
//   }
//   if (isotope == "e" || isotope == "electron") {
//     return nb::cast(Particle(isotope));
//   }
//   if (isotope == "n" || isotope == "neutron") {
//     return nb::cast(Particle(isotope));
//   }
  
//   return nb::cast(Ion(isotope));
// }