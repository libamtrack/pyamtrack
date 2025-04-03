#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <vector>
#include <stdexcept>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename Func>
auto wrap_function(Func func, const py::object& input) -> py::object {
    if (py::isinstance<py::float_>(input) || py::isinstance<py::int_>(input)) {
        // Single value input
        double value = input.cast<double>();
        return py::float_(func(value));
    } else if (py::isinstance<py::list>(input)) {
        // List input
        std::vector<double> values = input.cast<std::vector<double>>();
        std::vector<double> results;
        results.reserve(values.size());
        for (const auto& value : values) {
            results.push_back(func(value));
        }
        return py::cast(results);
    } else if (py::isinstance<py::array>(input)) {
        // NumPy array input
        auto array = input.cast<py::array_t<double>>();
        py::array_t<double> result(array.size());
        auto r = result.mutable_unchecked<1>();
        auto a = array.unchecked<1>();
        for (ssize_t i = 0; i < array.size(); ++i) {
            r(i) = func(a(i));
        }
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers");
    }
}

#endif // WRAPPER_TEMPLATE_H