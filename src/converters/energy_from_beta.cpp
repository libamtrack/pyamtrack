#include "../wrapper_template.h"
#include "energy_from_beta.h"

extern "C" {
    #include "AT_PhysicsRoutines.h"
}

py::object energy_from_beta(py::object input) {
    return wrap_function(AT_E_from_beta_single, input);
}
