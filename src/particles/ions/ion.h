#pragma once
#include "../particles.h"

class Ion : public Particle {
 private:
  friend nb::object from_string(const std::string& name);
  friend Ion from_ZA(long long Z, long long A);
  
  /**
   * @brief Initializes an Ion from an isotope string.
   *
   * Use from_string() instead to create Ion objects.
   *
   * @param isotope The isotope string (e.g., "12C", "He").
   */
  Ion(const std::string& isotope);

 public:
  long Z;  /**< Atomic number of the ion. */
  long A;  /**< Mass number of the ion. */
};
