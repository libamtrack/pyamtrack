#include <pybind11/pybind11.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

extern "C" {
    #include "AT_PhysicsRoutines.h"  // Include libamtrack header
    #include "AT_ElectronRange.h"  // Include libamtrack header

}

// Wrapper function for AT_beta_from_E_single (from libamtrack)
double beta_from_energy(double E_MeV_u) {
    return AT_beta_from_E_single(E_MeV_u);
}

double electron_range(double E_MeV_u) {
    const double range = AT_max_electron_range_m(E_MeV_u, 1, 7);
    return range;
}

// Additional custom function not using libamtrack
double calculate_velocity(double beta) {
    const double speed_of_light = 299792458.0;  // Speed of light in m/s
    return beta * speed_of_light;
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "Python bindings for libamtrack";

    // Expose the beta_from_energy function
    m.def("beta_from_energy", &beta_from_energy, "Calculate beta from energy per nucleon (MeV/u)");

    // Expose the calculate_velocity function
    m.def("calculate_velocity", &calculate_velocity, "Calculate velocity from beta (m/s)");

    // Expose the electron_range function
    m.def("electron_range", &electron_range, "Calculate electron range (m)");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif

}
