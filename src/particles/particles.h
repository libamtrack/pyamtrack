#ifndef PARTICLE_H
#define PARTICLE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

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
 * >>> particle.n
 * 6
 * >>> particle.element_name
 * 'Carbon'
 *
 * Attributes:
 * - n (long): The number of particle.
 * - Z (long): Atomic number of the particle.
 * - atomic_weight (double): Atomic weight of the particle.
 * - element_name (std::string): Name of the particle.
 * - element_acronym (std::string): Acronym of the particle.
 * - density_g_cm3 (double): Density of the particle in g/cm³.
 * - I_eV_per_Z (double): Mean ionization potential per atomic number in eV/Z.
 */
class Particle {
 public:
  long n;                      /**< The number of particle. */
  long Z;                      /**< Atomic number of the particle. */
  double atomic_weight;        /**< Atomic weight of the particle. */
  std::string element_name;    /**< Name of the particle. */
  std::string element_acronym; /**< Acronym of the particle. */
  double density_g_cm3;        /**< Density of the particle in g/cm³. */
  double I_eV_per_Z;           /**< Mean ionization potential per atomic number in eV/Z. */
  /**
   * @brief Initializes a Particle object.
   *
   * Example:
   * >>> particle = Particle(6)
   * >>> particle.n
   * 6
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param n The number of particle.
   */
  Particle(long n);

  /**
   * @brief Initializes a Particle object using its acronym.
   *
   * Example:
   * >>> particle = Particle("C")
   * >>> particle.n
   * 6
   * >>> particle.element_name
   * 'Carbon'
   *
   * @param acronym The acronym of the particle.
   */
  Particle(const std::string& acronym);
};

#endif  // MATERIALS_H
