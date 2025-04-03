#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <vector>
#include <stdexcept>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// Template function to handle single input and generate single output
// Supports double, list, and numpy array inputs
template <typename Func>
py::object wrap_function(Func func, const py::object& input) {
    if (py::isinstance<py::float_>(input) || py::isinstance<py::int_>(input)) {
        // Single value input
        double value = input.cast<double>();
        return py::float_(func(value));
    } else if (py::isinstance<py::list>(input)) {
        // List input
        py::list inputs = input.cast<py::list>();
        py::list result;
        for (auto i : inputs) {
            double x = py::cast<double>(i);
            result.append(func(x));
        }
        return result;
    } else if (py::isinstance<py::array>(input)) {
        // NumPy array input
        auto inputs = input.cast<py::array_t<double>>();
        auto r = inputs.unchecked<1>();
        py::array_t<double> result(inputs.size());
        auto res = result.mutable_unchecked<1>();
        for (std::ptrdiff_t i = 0; i < r.size(); i++) {
            res(i) = func(r(i));
        }
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers");
    }
}

#endif // WRAPPER_TEMPLATE_H