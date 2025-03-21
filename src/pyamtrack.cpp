#include <pybind11/pybind11.h>
#include <cmath>  // For additional mathematical operations

namespace py = pybind11;

extern "C" {
    #include "AT_PhysicsRoutines.h"  // Include libamtrack header
}

// // Wrapper function for AT_beta_from_E_single (from libamtrack)
// double beta_from_energy(double E_MeV_u) {
//     return AT_beta_from_E_single(E_MeV_u);
// }

// Additional custom function not using libamtrack
double calculate_velocity(double beta) {
    const double speed_of_light = 299792458.0;  // Speed of light in m/s
    return beta * speed_of_light;
}

PYBIND11_MODULE(pyamtrack, m) {
    m.doc() = "Python bindings for libamtrack and custom functions";

    // Expose the beta_from_energy function
    // m.def("beta_from_energy", &beta_from_energy, "Calculate beta from energy per nucleon (MeV/u)");

    // Expose the calculate_velocity function
    m.def("calculate_velocity", &calculate_velocity, "Calculate velocity from beta (m/s)");
}
