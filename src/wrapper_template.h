#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <vector>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <string>
#include <iostream>
#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

// Define the type for the function to be wrapped.
using Func = std::function<double(double)>;

// The wrapper function
inline nb::object wrap_function(Func func, const nb::object& input) {
    // 1. Check for scalar types (float or int)
    if (PyFloat_Check(input.ptr()) || PyLong_Check(input.ptr())) {
        double input_val = nb::cast<double>(input);
        double result = func(input_val);
        return nb::cast(static_cast<double>(result));
    }
    // 2. Check for Python list
    else if (nb::isinstance<nb::list>(input)) {
        nb::list py_list = nb::cast<nb::list>(input);
        std::vector<double> results;
        results.reserve(nb::len(py_list));

        for (nb::handle item : py_list) {
            if (!PyFloat_Check(item.ptr()) && !PyLong_Check(item.ptr())) {
                throw nb::type_error("List elements must be float or int.");
            }
            double item_val = nb::cast<double>(item);
            results.push_back(func(item_val));
        }
        return nb::cast(results);
    }
    // 3. Check for NumPy array
    else if (nb::isinstance<nb::ndarray<>>(input)) {
        
        // Handle ndarray with arbitrary dimension
        try {
            // Cast the input to the ndarray of type double 
            // (const because the input data is read only)
            auto input_array = nb::cast<nb::ndarray<const double>>(input);

            // Size is the total number of elements in the array
            size_t num_elements = input_array.size();
            
            // The shape vector is later used to initialize the result ndarray
            // It will have similar shape as input array
            std::vector<size_t> result_shape(input_array.ndim());
            for (size_t i = 0; i < input_array.ndim(); ++i)
                result_shape[i] = input_array.shape(i);
            
            // Get the pointer to the block of data to be mapped
            const double* data_buffer = input_array.data();

            // Initialize the vector that will store the result
            // And map all the elements from the input with the given func
            std::vector<double> results(num_elements);
            for(size_t i = 0; i < num_elements; ++i) {
                results[i] = func(data_buffer[i]);
            }
            
            // Create the result ndarray, with the mapped data and pass the according shape
            auto result_array = nb::ndarray<double, nb::numpy>(results.data(), result_shape.size(), result_shape.data()).cast();
            return result_array;

        } catch (const nb::cast_error& e) {
            throw nb::type_error("NumPy array dtype cannot be cast to double or input is not suitable.");
        } catch (const std::exception& e) {
            throw std::runtime_error("Error processing NumPy array: " + std::string(e.what()));
        }
    }

    // 4. Handle unsupported types
    throw nb::type_error("Input must be a float, int, list or NumPy array.");
}

#endif // WRAPPER_TEMPLATE_H