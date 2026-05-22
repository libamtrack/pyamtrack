#pragma once
#include "../particles.h"

class Ion : public Particle {
 public:
  long Z;  /**< Atomic number of the ion. */
  long A;  /**< Mass number of the ion. */

  /**
   * @brief Initializes an Ion from an isotope string.
   *
   * @param isotope The isotope string (e.g., "12C", "He").
   */
  Ion(const std::string& isotope);
};
