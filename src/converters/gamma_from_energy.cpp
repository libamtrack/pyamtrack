#include "gamma_from_energy.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object gamma_from_energy(nb::object energy_MeV_u) {
  return wrap_function(AT_gamma_from_E_single, energy_MeV_u);
}
