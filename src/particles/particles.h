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
 * @brief Retrieve the list of particle names.
 *
 * Example:
 * >>> get_names()
 * ["Hydrogen", "Helium", "Lithium", ..., "Californium"]
 *
 * @return std::vector<std::string> List of particle names.
 */
std::vector<std::string> get_names();

/**
 * @brief Retrieve the list of particle acronyms.
 *
 * Example:
 * >>> get_acronyms()
 * ["H", "He", "Li", ..., "Cf"]
 *
 * @return std::vector<std::string> List of particle acronyms.
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
 * - id (long): Internal particle ID (row index in AT_Particle_Data).
 * - pdg (long long): PDG code.
 * - atomic_weight (double): Atomic weight.
 * - element_name (std::string): Element name.
 * - element_acronym (std::string): Element acronym.
 */
class Particle {


 public:
   /**
    * @brief Construct a Particle from a PDG code.
    *
    * For ion PDG codes, this constructor throws and advises using Ion instead.
    *
    * @param pdg_code PDG code.
    * @throws std::invalid_argument If the PDG code is invalid or corresponds to an ion.
    */
  Particle(long long pdg_code);
  Particle();

  long id;                     /**< The id of particle. */
  long long pdg; 
  double atomic_weight;        /**< Atomic weight of the particle. */
  std::string element_name;    /**< Name of the particle. */
  std::string element_acronym; /**< Acronym of the particle. */
  
  // 
  //double density_g_cm3;        /**< Density of the particle in g/cm³. */
  //double I_eV_per_Z;           /**< Mean ionization potential per atomic number in eV/Z. */
  
   /**
    * @brief Short string representation.
    *
    * @return std::string Element acronym.
    */
   std::string str() const;

   /**
    * @brief Detailed string representation.
    *
    * @return std::string Debug-friendly representation.
    */
   std::string repr() const;


   /**
    * @brief Construct a Particle or Ion from a string representation.
    *
    * The string can be:
    *   - Element acronym ("He", "C", "U")
    *   - Isotope notation ("3He", "4He", "12C", "238U")
    *   - Special names "proton", "alpha", "neutron", "electron"
    */
};

#include "ions/ion.h"


/**
 * @brief Create an Ion from atomic number Z and mass number A.
 *
 * @param Z Atomic number.
 * @param A Mass number.
 * @return Ion
 * @throws std::invalid_argument If the element or isotope is not found.
 */
Ion from_ZA(long long Z, long long A);



/**
 * @brief Create a Particle or Ion from a string label.
 *
 * @param name Element acronym, isotope label, or special name.
 * @return nb::object Particle or Ion instance.
 * @throws std::invalid_argument or nb::value_error on invalid input.
 */
nb::object from_string(const std::string& name);

/**
 * @brief Create a non-ion special particle from a name.
 *
 * @param name Supported names: "neutron", "electron".
 * @return Particle
 */
Particle create_particle(const std::string& name);

/**
 * @brief Create an ion from an element or isotope string.
 *
 * @param name Element acronym/name or isotope label.
 * @return Ion
 * @throws nb::value_error If the element or mass number is invalid.
 */
Ion create_ion(const std::string& name);

/**
 * @brief Create a Particle or Ion from a PDG code.
 *
 * Handles proton, neutron, and electron PDG codes explicitly.
 *
 * @param pdg_code PDG code.
 * @return nb::object Particle or Ion instance.
 * @throws nb::value_error If the code is not a valid nuclear PDG encoding.
 */
nb::object from_pdg(long long pdg_code);

#endif  // PARTICLE_H
