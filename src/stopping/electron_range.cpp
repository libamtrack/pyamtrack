#include "electron_range.h"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <stdexcept>

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
    if (py::isinstance<py::float_>(input) || py::isinstance<py::int_>(input)) {
        double energy = input.cast<double>();
        return py::float_(electron_range_single(energy));
    } else if (py::isinstance<py::array_t<double>>(input)) {
        auto energies = input.cast<py::array_t<double>>();
        auto r = energies.unchecked<1>();
        py::array_t<double> result(energies.size());
        auto res = result.mutable_unchecked<1>();

        for (std::ptrdiff_t i = 0; i < r.size(); i++) {
            res(i) = electron_range_single(r(i));
        }

        return result;
    } else if (py::isinstance<py::list>(input)) {
        py::list energies = input.cast<py::list>();
        py::list result;
        for (auto energy : energies) {
            double e = py::cast<double>(energy);
            result.append(electron_range_single(e));
        }
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers.");
    }
}