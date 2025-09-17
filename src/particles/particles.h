#ifndef PARTICLE_H
#define PARTICLE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <optional>
#include <string>
#include <vector>

extern "C" {
#include "AT_DataParticle.h"
}

namespace nb = nanobind;

/**
 * @brief Retrieves the list of particles names.
 *
 * Example:
 * >>> get_names()
 * ["Hydrogen", "Helium", "Lithium", ..., "Californium"]
 *
 * @return std::vector<std::string> A list of particle names.
 */
std::vector<std::string> get_names();

/**
 * @brief Retrieves the list of particles acronyms.
 *
 * Example:
 * >>> get_acronyms()
 * ["H", "He", "Li", ..., "Cf"]
 *
 * @return std::vector<std::string> A list of particle acronyms.
 */
std::vector<std::string> get_acronyms();

/**
 * @class Particle
 * @brief Represents a particle with various physical properties.
 *
 * Example:
 * >>> particle = Particle(6)
 * >>> particle.id
 * 6
 * >>> particle.element_name
 * 'Carbon'
 *
 * Attributes:
 * - id (long): The id of particle.
 * - Z (long): Atomic number of the particle.
 * - A (optional long): Mass number of the particle.
 * - atomic_weight (double): Atomic weight of the particle.
 * - element_name (std::string): Name of the particle.
 * - element_acronym (std::string): Acronym of the particle.
 * - density_g_cm3 (double): Density of the particle in g/cm³.
 * - I_eV_per_Z (double): Mean ionization potential per atomic number in eV/Z.
 */
class Particle {
 public:
  long id;                     /**< The id of particle. */
  long Z;                      /**< Atomic number of the particle. */
  std::optional<long> A;       /**< Mass number of the particle. */
  double atomic_weight;        /**< Atomic weight of the particle. */
  std::string element_name;    /**< Name of the particle. */
  std::string element_acronym; /**< Acronym of the particle. */
  double density_g_cm3;        /**< Density of the particle in g/cm³. */
  double I_eV_per_Z;           /**< Mean ionization potential per atomic number in eV/Z. */
  nb::object py_get_A() const;
  /**
   * @brief Initializes a Particle object.
   *
   * Example:
   * >>> particle = Particle(6)
   * >>> particle.id
   * 6
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param id The number of particle.
   */
  Particle(long id);

  /**
   * @brief Initializes a Particle object using its acronym.
   *
   * Example:
   * >>> particle = Particle("C")
   * >>> particle.id
   * 6
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param acronym The acronym of the particle.
   */
  Particle(const std::string& acronym);

  /**
   * @brief Initializes a Particle object from a particle number (1000*Z + A).
   *
   * A particle number encodes the atomic number (Z) and mass number (A)
   * according to the convention:
   *
   *   particle_no = 1000 * Z + A
   *
   * where:
   *   - Z is the atomic number (1 ≤ Z ≤ 118)
   *   - A is the mass number (1 ≤ A ≤ 300)
   *
   * Example:
   * >>> particle = Particle::from_number(6012)
   * >>> particle.Z
   * 6
   * >>> particle.A
   * 12
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param particle_no The particle number in the format 1000*Z + A.
   * @return A Particle object corresponding to the given particle number.
   * @throws std::invalid_argument if the particle number is invalid.
   */
  static Particle from_number(long particle_no);

  /**
   * @brief Initializes a Particle object from a string representation.
   *
   * The string can be:
   *   - Just the acronym of the element: "He", "C", "U"
   *   - An isotope notation with mass number: "3He", "4He", "12C", "238U"
   *
   * Example:
   * >>> particle = Particle::from_string("14C")
   * >>> particle.Z
   * 6
   * >>> particle.A
   * 14
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param name The string representation of the particle.
   * @return A Particle object corresponding to the given name.
   * @throws std::invalid_argument if the name cannot be parsed.
   */
  static Particle from_string(const std::string& name);
};

#endif  // PARTICLE_H
