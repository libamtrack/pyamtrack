#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <vector>
#include <stdexcept>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

// Template function to handle single input and generate single output
// Supports double, list, and numpy array inputs
template <typename Func>
nb::object wrap_function(Func func, const nb::object& input) {
    if (nb::isinstance<nb::float_>(input) || nb::isinstance<nb::int_>(input)) {
        // Single value input
        double value = nb::cast<double>(input);
        return nb::float_(func(value));
    } else if (nb::isinstance<nb::list>(input)) {
        // List input
        nb::list inputs = nb::cast<nb::list>(input);
        nb::list result = nb::list();
        for (auto i : inputs) {
            double x = nb::cast<double>(i);
            result.append(func(x));
        }
        return result;
    } else if (nb::isinstance<nb::ndarray<>>(input)) {
        // NumPy array input
        nb::ndarray<double, nb::shape<nb::any>> inputs = nb::cast<nb::ndarray<double>>(input);
        
        if (inputs.ndim() != 1) {
            throw std::invalid_argument("Expected a 1D array");
        }
        
        size_t size = inputs.shape(0);
        auto result = nb::ndarray<double, nb::shape<nb::any>>(
            nb::shape(size), nb::device::cpu::value
        );
        
        // Process each element
        auto inputs_data = inputs.data();
        auto result_data = result.data();
        for (size_t i = 0; i < size; i++) {
            result_data[i] = func(inputs_data[i]);
        }
        
        return result;
    } else {
        throw std::invalid_argument("Input must be a float, int, NumPy array, or a Python list of numbers");
    }
}

#endif // WRAPPER_TEMPLATE_H