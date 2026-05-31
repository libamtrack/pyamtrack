#pragma once
#include "../particles.h"

class Ion : public Particle {
 public:
  /**
   * @brief Initializes an Ion from an isotope string.
   *
   * Use from_string() instead to create Ion objects.
   *
   * @param isotope The isotope string (e.g., "12C", "He").
   */
  Ion(long long pdg_code);
  Ion();

  long Z;  /**< Atomic number of the ion. */
  long A;  /**< Mass number of the ion. */
};
