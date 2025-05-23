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
inline nb::object wrap_function(Func func, const nb::object &input) {
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

        for (nb::handle item: py_list) {
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
                for (size_t i = 0; i < size; ++i) {
                    results[i] = func(input_array(i));
                }
                auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {size}).cast();
                return result_array;

            } catch (const nb::cast_error &e) {
                throw nb::type_error("1-D NumPy array dtype cannot be cast to double or input is not suitable.");
            } catch (const std::exception &e) {
                throw std::runtime_error("Error processing 1-D NumPy array: " + std::string(e.what()));
            }
        }
    }

    // 4. Handle unsupported types
    throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
}

inline nb::object wrap_multiargument_function(const MultiargumentFunc &func, const std::vector<nb::object> &input) {

    // Check for scalar types (float or int)
    bool scalars_only = true;
    for (const auto& argument: input) {
        if (!PyFloat_Check(argument.ptr()) && !PyLong_Check(argument.ptr())){
            scalars_only = false;
        }
    }
    if (scalars_only) {
        std::vector<double> input_casted;
        input_casted.reserve(input.size());
        for (const auto &argument: input) {
            input_casted.push_back(nb::cast<double>(argument));
        }
        double result = func(input_casted);
        return nb::cast(static_cast<double>(result));
    }
        // Check for Python list and / or arrays
    else {
        std::vector<int> argument_types; // -1 for scalar, array_arguments idx otherwise
        argument_types.reserve(input.size());
        std::vector<nb::ndarray<const double, nb::shape<-1>>> array_arguments;
        std::vector<nb::list> list_arguments;
        size_t len = 0;
        for (const auto & i : input) {
            if (PyFloat_Check(i.ptr()) || PyLong_Check(i.ptr()))
                argument_types.push_back(-1);
            else if (nb::isinstance<nb::list>(i)) {
                argument_types.push_back((int) list_arguments.size());
                auto list = nb::cast<nb::list>(i);
                list_arguments.push_back(list);
                size_t new_len = nb::len(list);
                if (len && new_len != len)
                    throw nb::value_error("Incompatible lists/arrays size");
                else
                    len = new_len;
            }
            else if (nb::isinstance<nb::ndarray<>>(i)) {
                argument_types.push_back((int) array_arguments.size());
                auto array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(i);
                if (array.ndim() != 1) {
                    throw nb::value_error("Input NumPy array must be 1-D.");
                }
                array_arguments.push_back(array);
                size_t new_len = array.size();
                if (len && new_len != len)
                    throw nb::value_error("Incompatible lists/arrays size");
                else
                    len = new_len;
            }else {
                // Handle unsupported types
                throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
            }
        }
        std::vector<double> results(len);
        try {
            for (int i = 0; i < len; i++) {
                std::vector<double> argument_list;
                for (int j = 0; j < argument_types.size(); j++) {
                    if (argument_types[j] == -1)
                        argument_list.push_back(nb::cast<double>(input[j]));
                    else if(nb::isinstance<nb::list>(input[j])){
                        argument_list.push_back(nb::cast<double>(list_arguments[argument_types[j]][i]));
                    }
                    else{
                        argument_list.push_back(array_arguments[argument_types[j]](i));
                    }
                }
                results[i] = func(argument_list);
            }
        } catch (const nb::cast_error &e) {
            throw nb::type_error("1-D NumPy array dtype cannot be cast to double or input is not suitable.");
        } catch (const std::exception &e) {
            throw std::runtime_error("Error processing 1-D NumPy array: " + std::string(e.what()));
        }

        auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {len}).cast();
        return result_array;
    }


}

#endif // WRAPPER_TEMPLATE_H