#pragma once
#include <stdexcept>
#include <string>

#include "../particles.h"

/**
 * @brief Represents an ion with atomic number Z and mass number A.
 */
class Ion : public Particle {
 public:
  /**
   * @brief Construct an Ion from a PDG code.
   *
   * @param pdg_code PDG code.
   * @throws std::invalid_argument If the PDG code is invalid or non-ion.
   */
  Ion(long long pdg_code);
  /**
   * @brief Default constructor for Ion.
   */
  Ion();

  long Z; /**< Atomic number of the ion. */
  long A; /**< Mass number of the ion. */

  /**
   * @brief libamtrack particle number (1000*Z + A).
   *
   * Used when passing ions into calculation functions such as stopping power.
   */
  long get_particle_no() const {
    if (Z < 1 || A < 1) {
      throw std::invalid_argument("Ion is missing a valid Z and A");
    }
    return AT_particle_no_from_Z_and_A_single(Z, A);
  }
};
