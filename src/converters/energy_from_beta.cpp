#include "../wrapper_template.h"
#include "energy_from_beta.h"

extern "C" {
    #include "AT_PhysicsRoutines.h"
}

nb::object energy_from_beta(nb::object input) {
    return wrap_function(AT_E_from_beta_single, input);
}
