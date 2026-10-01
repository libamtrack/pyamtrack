#include "energy_from_momentum.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object energy_from_momentum(nb::object energy_MeV_u) {
  return wrap_function(AT_E_MeV_u_from_momentum_single, energy_MeV_u );
}