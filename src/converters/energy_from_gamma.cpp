#include "energy_from_gamma.h"

#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object energy_from_gamma(nb::object gamma) {
  return wrap_function(AT_E_from_gamma_single, gamma);
}