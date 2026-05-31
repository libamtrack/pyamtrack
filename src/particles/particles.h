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

// forward declaration
class Ion;

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
 * - atomic_weight (double): Atomic weight of the particle.
 * - element_name (std::string): Name of the particle.
 * - element_acronym (std::string): Acronym of the particle.
 * - density_g_cm3 (double): Density of the particle in g/cm³.
 * - I_eV_per_Z (double): Mean ionization potential per atomic number in eV/Z.
 */
class Particle {


 public:
  /**
   * @brief Initializes a Particle object from an isotope string.
   *
   * Use from_string() instead to create Particle objects.
   *
   * @param isotope The isotope string (e.g., "12C", "He", "238U").
   */
  Particle(long long pdg_code);
  Particle();

  long id;                     /**< The id of particle. */
  std::optional<long long> pdg; 
  double atomic_weight;        /**< Atomic weight of the particle. */
  std::string element_name;    /**< Name of the particle. */
  std::string element_acronym; /**< Acronym of the particle. */
  
  // 
  //double density_g_cm3;        /**< Density of the particle in g/cm³. */
  //double I_eV_per_Z;           /**< Mean ionization potential per atomic number in eV/Z. */
  
  long py_get_A() const;
  nb::object py_get_pdg() const;
  nb::object py_get_charge() const;
  std::string str() const;
  std::string repr() const;


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

};

#include "ions/ion.h"

// /**
//  * @brief Factory function that creates either a Particle or Ion based on the isotope string.
//  *
//  * For special particles (proton, electron, neutron), returns a Particle.
//  * For regular atoms/ions, returns an Ion with Z and A set.
//  *
//  * Example:
//  * >>> ion = create_particle("12C")  # Returns Ion
//  * >>> p = create_particle("p")      # Returns Particle (proton)
//  *
//  * @param isotope The isotope string (e.g., "12C", "He", "p", "e", "n").
//  * @return nb::object A Particle or Ion object.
//  */
// nb::object create_particle(const std::string& isotope);

Ion from_ZA(long long Z, long long A);


Ion from_ZA(long long Z, long long A);


nb::object from_string(const std::string& name);

Particle create_particle(const std::string& name);
Ion create_ion(const std::string& name);

nb::object from_pdg(long long pdg_code);

#endif  // PARTICLE_H
