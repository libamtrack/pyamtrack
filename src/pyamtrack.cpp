#include <pybind11/pybind11.h>
#include "AT_Constants.h"  // Include necessary libamtrack headers

extern "C" double AT_beta_from_E_single(double E_MeV_u);  // Declare the function explicitly as C linkage

namespace py = pybind11;

// Wrapper function for AT_beta_from_E_single
double beta_from_energy(double E_MeV_u) {
    return AT_beta_from_E_single(E_MeV_u);
}

PYBIND11_MODULE(pyamtrack, m) {
    m.doc() = "Python bindings for libamtrack";

    // Expose the beta function
    m.def("beta_from_energy", &beta_from_energy, "Calculate beta from energy per nucleon (MeV/u)");
}
