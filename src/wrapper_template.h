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
using MultiargumentFunc = std::function<double(std::vector<double>)>;

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
        // Get generic handle only for checking properties like ndim
        auto np_array_generic = nb::cast<nb::ndarray<>>(input);

        if (np_array_generic.ndim() != 1) {
            throw nb::value_error("Input NumPy array must be 1-D.");
        }

        // Handle 1-D array
        if (np_array_generic.ndim() == 1) {
            try {
                auto input_array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(input);
                size_t size = input_array.size();
                std::vector<double> results(size);
                for(size_t i = 0; i < size; ++i) {
                    results[i] = func(input_array(i));
                }
                auto result_array = nb::ndarray<double, nb::numpy>(results.data() ,{size}).cast();
                return result_array;

            } catch (const nb::cast_error& e) {
                throw nb::type_error("1-D NumPy array dtype cannot be cast to double or input is not suitable.");
            } catch (const std::exception& e) {
                throw std::runtime_error("Error processing 1-D NumPy array: " + std::string(e.what()));
            }
        }
    }

    // 4. Handle unsupported types
    throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
}

inline nb::object wrap_multiargument_function(const MultiargumentFunc& func, const std::vector<nb::object>& input) {
    // 1. Check for scalar types (float or int)
    if (input.empty() || PyFloat_Check(input[0].ptr()) || PyLong_Check(input[0].ptr())) {
        for (const auto &argument: input) {
            if (!PyFloat_Check(argument.ptr()) && !PyLong_Check(argument.ptr())) {
                throw nb::type_error("all arguments must be float or int.");
            }
        }
        std::vector<double> input_casted;
        input_casted.reserve(input.size());
        for (const auto &argument: input) {
            input_casted.push_back(nb::cast<double>(argument));
        }
        double result = func(input_casted);
        return nb::cast(static_cast<double>(result));
    }
        // 2. Check for Python list
    else if (nb::isinstance<nb::list>(input[0])) {
        std::vector<nb::list> input_casted;
        input_casted.reserve(input.size());
        for (const auto& argument: input) {
            input_casted.push_back(nb::cast<nb::list>(argument));
        }
        std::vector<double> results;
        results.reserve(nb::len(input_casted[0]));
        for (int i = 0; i < input_casted[0].size(); i++) {
            std::vector<double> arguments;
            for (const auto &j: input_casted) {
                nb::handle item = j[i];
                if (!PyFloat_Check(j[i].ptr()) && !PyLong_Check(j[i].ptr())) {
                    throw nb::type_error("List elements must be float or int.");
                }
                arguments.push_back(nb::cast<double>(item));
            }
            results.push_back(func(arguments));
        }
        return nb::cast(results);
    }
        // 3. Check for NumPy array
    else if (nb::isinstance<nb::ndarray<>>(input[0])) {
        // Get generic handle only for checking properties like ndim
        std::vector<nb::ndarray<const double, nb::shape<-1>>> arguments_casted;
        for (const auto& argument: input) {
            auto arg_casted = nb::cast<nb::ndarray<>>(argument);
            if (arg_casted.ndim() != 1) {
                throw nb::value_error("Input NumPy array must be 1-D.");
            }
            auto arg = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(argument);
            arguments_casted.push_back(arg);
        }

        // Handle 1-D array
        try {
            //auto input_arrays = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(input);
            size_t size = arguments_casted[0].size();
            std::vector<double> results(size);
            for (size_t i = 0; i < arguments_casted[0].size(); i++) {
                std::vector<double> arguments;
                for (const auto &j: arguments_casted) {
                    auto item = j(i);
                    arguments.push_back(item);
                }
                results[i] = func(arguments);
            }
            auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {size}).cast();
            return result_array;

        } catch (const nb::cast_error &e) {
            throw nb::type_error("1-D NumPy array dtype cannot be cast to double or input is not suitable.");
        } catch (const std::exception &e) {
            throw std::runtime_error("Error processing 1-D NumPy array: " + std::string(e.what()));
        }

    }

    // 4. Handle unsupported types
    throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
}

#endif // WRAPPER_TEMPLATE_H