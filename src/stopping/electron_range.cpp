#include "../wrapper_template.h"
#include "electron_range.h"

extern "C" {
    #include "AT_ElectronRange.h"
}

namespace py = pybind11;

// Helper function for single value calculation
double electron_range_single(double E_MeV_u) {
    return AT_max_electron_range_m(E_MeV_u, 1, 7);
}

// Main function to handle different input types
py::object electron_range(py::object input) {
    return wrap_function(electron_range_single, input);
}