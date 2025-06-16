#include "beta_from_energy.h"

#include "../wrapper_template.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object beta_from_energy(nb::object energy_per_nucleon_MeV_u) {
  return wrap_function(AT_beta_from_E_single, energy_per_nucleon_MeV_u);
}
