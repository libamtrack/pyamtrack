#include "beta_from_energy.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object beta_from_energy(nb::object energy_MeV_u) {
  return wrap_function(AT_beta_from_E_single, energy_MeV_u);
}
