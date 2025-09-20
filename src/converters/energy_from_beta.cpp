#include "energy_from_beta.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object energy_from_beta(nb::object beta) {
  return wrap_function(AT_E_from_beta_single, beta);
}
