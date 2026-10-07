#ifndef PARTICLE_TO_LONG_H
#define PARTICLE_TO_LONG_H

#include <stdexcept>

#include "ions/ion.h"

extern "C" {
#include "AT_DataParticle.h"
}

/**
 * @brief Convert an ion to the particle identifier expected by the C backend.
 *
 * This is the single boundary between the C++ particle representation and the
 * identifier representation used by libamtrack. If the backend changes to use
 * another identifier, update this function only.
 *
 * @param ion Ion to convert.
 * @return The C backend particle identifier.
 * @throws std::invalid_argument If the ion does not contain valid Z and A.
 */
inline long particle_to_long(const Ion& ion) {
  if (ion.Z < 1 || ion.A < 1) {
    throw std::invalid_argument("Ion is missing a valid Z and A");
  }

  return AT_particle_no_from_Z_and_A_single(ion.Z, ion.A);
}

#endif  // PARTICLE_TO_LONG_H
