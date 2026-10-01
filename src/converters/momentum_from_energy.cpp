#include "momentum_from_energy.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object momentum_from_energy(nb::object energy_MeV_u) {
  return wrap_function(AT_momentum_from_E_MeV_c_u_single, energy_MeV_u);
}