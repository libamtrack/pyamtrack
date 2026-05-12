#include "particles.h"
#include "construct_utils.h"

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

Particle::Particle(const std::string& isotope) {
  const auto& data = AT_Particle_Data;
  auto [symbol, A_] = parse_isotope(isotope);

  std::string acronym;
  std::string element_name;

  // if (symbol == "proton") {
  //   symbol = "H";
  //   A_ = 1;
  // } else if (symbol == "alpha") {
  //   symbol = "He";
  //   A_ = 4;
  // } else {

  // }

  
  int it = -1;
  for (int i = 0; i < data.n; ++i) {
    // std::cout << "Comparing symbol '" << to_lower_case(symbol) << "' with element acronym '" << to_lower_case(data.element_acronym[i]) << "'\n";
    if (to_lower_case(data.element_acronym[i]) == to_lower_case(symbol)) {
      it = i;

      break;
    }
    // std::cout << "Comparing symbol '" << to_lower_case(symbol) << "' with element acronym '" << to_lower_case(data.element_name[i]) << "'\n";
    if (to_lower_case(data.element_name[i]) == to_lower_case(symbol)) {
      it = i;
      break;
    }
  }


  if (it == -1) {
    // nb
    throw std::invalid_argument("Unknown element name or acronym: " + symbol);
  }

  acronym = data.element_acronym[it];
  element_name = data.element_name[it];

  if (A_ == -1) {
    A_ = most_popular_iso_A[acronym];
  }
  A = A_;
  id = it + 1;
  Z = data.Z[it];
  atomic_weight = data.atomic_weight[it];
  element_name = std::string(data.element_name[it]);
  element_acronym = acronym;
  // density_g_cm3 = data.density_g_cm3[it];
  // I_eV_per_Z = data.I_eV_per_Z[it];
  pdg = std::make_optional<long long>(calculatePDG(Z, A));

  // std::cout << "[Particle DEBUG] Construction successful:\n"
  //         << "  element = " << element_acronym << " (" << element_name << ")\n"
  //         << "  Z = " << Z << ", A = " << A << "\n"
  //         << "  id = " << id << "\n"
  //         // << "  PDG = " << pdg << "\n"
  //         << "  density = " << density_g_cm3 << " g/cm^3\n"
  //         << "  I/Z = " << I_eV_per_Z << " eV\n"
  //         << std::endl;
}

Particle Particle::from_number(long particle_no) {
  long Z_candidate = particle_no / 1000;
  long A_candidate = AT_A_from_particle_no_single(particle_no);

  if (A_candidate < 0) {
    throw std::invalid_argument("Invalid particle number: " + std::to_string(particle_no));
  }

  const auto& data = AT_Particle_Data;
  for (int i = 0; i < data.n; ++i) {
    if (data.Z[i] == Z_candidate) {
      std::string acronym(data.element_acronym[i]);
      Particle p(acronym);
      p.A = A_candidate;
      return p;
    }
  }

  throw std::invalid_argument("Particle with Z=" + std::to_string(Z_candidate) + " not found");
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
Particle Particle::from_string(const std::string& name) {
  size_t pos = 0;

  // Step 1: parse leading digits (mass number, e.g. "12" in "12C")
  while (pos < name.size() && isdigit(name[pos])) {
    ++pos;
  }

  std::optional<long> mass_number;
  if (pos > 0) {
    mass_number = std::stol(name.substr(0, pos));
  }

  // Step 2: extract the element acronym
  std::string acronym = name.substr(pos);

  if (acronym.empty()) {
    throw std::invalid_argument("Invalid particle name: " + name);
  }

  // Step 3: construct particle from acronym
  Particle p(acronym);

  // Step 4: attach mass number if one was parsed
  if (mass_number) {
    p.A = *mass_number;
  }

  return p;
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
  return A;
}

std::string Particle::str() const {
  return std::to_string(A) + element_acronym;
}