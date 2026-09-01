#pragma once
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

  long Z;  /**< Atomic number of the ion. */
  long A;  /**< Mass number of the ion. */
};
