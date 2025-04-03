#include "energy_from_beta.h"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <stdexcept>

extern "C" {
    #include "AT_PhysicsRoutines.h"
}

namespace py = pybind11;

double energy_from_beta_single(double beta) {
    return AT_E_from_beta_single(beta);
}

py::object energy_from_beta(py::object input) {
    if (py::isinstance<py::float_>(input) || py::isinstance<py::int_>(input)) {
        double beta = input.cast<double>();
        return py::float_(energy_from_beta_single(beta));
    } else if (py::isinstance<py::array_t<double>>(input)) {
        auto betas = input.cast<py::array_t<double>>();
        auto r = betas.unchecked<1>();
        py::array_t<double> result(betas.size());
        auto res = result.mutable_unchecked<1>();

        for (std::ptrdiff_t i = 0; i < r.size(); i++) {
            res(i) = energy_from_beta_single(r(i));
        }

        return result;
    } else if (py::isinstance<py::list>(input)) {
        py::list betas = input.cast<py::list>();
        py::list result;
        for (auto beta : betas) {
            double b = py::cast<double>(beta);
            result.append(energy_from_beta_single(b));
        }
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers.");
    }
}