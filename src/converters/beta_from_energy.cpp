#include "../wrapper_template.h"
#include "beta_from_energy.h"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cstddef>

#define PYBIND11_DETAILED_ERROR_MESSAGES


extern "C" {
    #include "AT_PhysicsRoutines.h"
}

namespace py = pybind11;

double beta_from_energy_single(double E_MeV_u) {
    return AT_beta_from_E_single(E_MeV_u);
}

py::object beta_from_energy(py::object input) {
    return wrap_function(beta_from_energy_single, input);
}