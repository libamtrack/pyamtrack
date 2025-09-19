#include "particles.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <stdexcept>

Particle::Particle(long id) : id(id) {
  if (id < 1 || id > AT_Particle_Data.n) {
    throw std::invalid_argument("Invalid particle id: " + std::to_string(id));
  }
  size_t index = static_cast<size_t>(id - 1);
  Z = AT_Particle_Data.Z[index];
  atomic_weight = AT_Particle_Data.atomic_weight[index];
  element_name = std::string(AT_Particle_Data.element_name[index]);
  element_acronym = std::string(AT_Particle_Data.element_acronym[index]);
  density_g_cm3 = AT_Particle_Data.density_g_cm3[index];
  I_eV_per_Z = AT_Particle_Data.I_eV_per_Z[index];
  A = std::nullopt;
}

Particle::Particle(const std::string& acronym) {
  const auto& data = AT_Particle_Data;
  auto it = std::find_if(data.element_acronym, data.element_acronym + data.n,
                         [&acronym](const char* a) { return acronym == a; });

  if (it == data.element_acronym + data.n) {
    throw std::invalid_argument("Particle acronym not found: " + acronym);
  }

  size_t index = std::distance(data.element_acronym, it);
  id = index + 1;
  Z = data.Z[index];
  atomic_weight = data.atomic_weight[index];
  element_name = std::string(data.element_name[index]);
  element_acronym = acronym;
  density_g_cm3 = data.density_g_cm3[index];
  I_eV_per_Z = data.I_eV_per_Z[index];
  A = std::nullopt;
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
      Particle p(i + 1);
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

nb::object Particle::py_get_A() const {
  if (A) {
    return nb::int_(*A);
  } else {
    return nb::none();
  }
}
