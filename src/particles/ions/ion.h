#pragma once
#include "../particles.h"

class Ion : public Particle {
 public:
  Ion(long long pdg_code);
  Ion();

  long Z;  /**< Atomic number of the ion. */
  long A;  /**< Mass number of the ion. */
};
