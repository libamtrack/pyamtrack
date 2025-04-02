#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>  // Include NumPy support
#include <pybind11/stl.h>    // Include support for Python lists
#include <cstddef>           // Include for std::ptrdiff_t

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

extern "C" {
    #include "AT_PhysicsRoutines.h"  // Include libamtrack header
    #include "AT_ElectronRange.h"    // Include libamtrack header
}

// Wrapper function for AT_beta_from_E_single (from libamtrack)
double beta_from_energy_single(double E_MeV_u) {
    return AT_beta_from_E_single(E_MeV_u);
}

py::object beta_from_energy(py::object input) {
    if (py::isinstance<py::float_>(input) || py::isinstance<py::int_>(input)) {
        // Handle single number
        double energy = input.cast<double>();
        return py::float_(beta_from_energy_single(energy));
    } else if (py::isinstance<py::array_t<double>>(input)) {
        // Handle NumPy array
        auto energies = input.cast<py::array_t<double>>();
        auto r = energies.unchecked<1>();  // Access the array without bounds checking
        py::array_t<double> result(energies.size());
        auto res = result.mutable_unchecked<1>();

        for (std::ptrdiff_t i = 0; i < r.size(); i++) {  // Use std::ptrdiff_t instead of ssize_t
            res(i) = beta_from_energy_single(r(i));
        }

        return result;
    } else if (py::isinstance<py::list>(input)) {
        // Handle Python list
        py::list energies = input.cast<py::list>();
        py::list result;
        for (auto energy : energies) {
            double e = py::cast<double>(energy);  // Explicitly cast to double
            result.append(beta_from_energy_single(e));
        }
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers.");
    }
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "Python bindings for libamtrack";

    // Expose the unified beta_from_energy function
    m.def("beta_from_energy", &beta_from_energy, "Calculate beta from energy per nucleon (MeV/u). Accepts a single value, a NumPy array, or a Python list.");

    // Expose other functions
    m.def("calculate_velocity", [](double beta) {
        const double speed_of_light = 299792458.0;  // Speed of light in m/s
        return beta * speed_of_light;
    }, "Calculate velocity from beta (m/s)");

    m.def("electron_range", [](double E_MeV_u) {
        return AT_max_electron_range_m(E_MeV_u, 1, 7);
    }, "Calculate electron range (m)");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}