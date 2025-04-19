#include "../wrapper_template.h"
#include "beta_from_energy.h"

extern "C" {
    #include "AT_PhysicsRoutines.h"
}

nb::object beta_from_energy(nb::object input) {
    return wrap_function(AT_beta_from_E_single, input);
}