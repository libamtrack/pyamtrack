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

inline nb::object prepare_array_argument(const nb::object &scalar, std::size_t shape){
    std::vector<double> results(shape);
    for(std::size_t i = 0; i < shape; i++){
        results[i] = nb::cast<double>(scalar);
    }
    auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {static_cast<std::size_t>(shape)}).cast();
    return result_array;
}

inline nb::object wrap_multiargument_function(const MultiargumentFunc &func, const std::vector<nb::object> &input) {

    // Check for scalar types (float or int)
    bool scalars_only = true;
    size_t len = 0;
    for (const auto& argument: input) {
        if (nb::isinstance<nb::list>(argument)) {
            auto list = nb::cast<nb::list>(argument);
            len = nb::len(list);
            scalars_only = false;
            break;
        }
        else if (nb::isinstance<nb::ndarray<>>(argument)) {
            auto array = nb::cast<nb::ndarray<>>(argument);
            len = array.size();
            scalars_only = false;
            break;
        }else if (!PyFloat_Check(argument.ptr()) && !PyLong_Check(argument.ptr())){
            // Handle unsupported types
            throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
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
        std::vector<nb::object> arguments;
        for (const auto & i : input) {
            if (PyFloat_Check(i.ptr()) || PyLong_Check(i.ptr()))
                arguments.push_back(prepare_array_argument(i, (int)len));
            else if (nb::isinstance<nb::list>(i)) {
                arguments.push_back(i);
                if (nb::len(nb::cast<nb::list>(i)) != len)
                    throw nb::value_error("Incompatible lists/arrays size");
            }
            else if (nb::isinstance<nb::ndarray<>>(i)) {
                auto array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(i);
                if (array.ndim() != 1)
                    throw nb::value_error("Input NumPy array must be 1-D.");
                arguments.push_back(i);
                if (array.size() != len)
                    throw nb::value_error("Incompatible lists/arrays size");
            }else {
                // Handle unsupported types
                throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
            }
        }
        std::vector<double> results(len);
        try {
            for (int i = 0; i < len; i++) {
                std::vector<double> arguments_vector;
                for (auto & argument : arguments) {
                    if(nb::isinstance<nb::list>(argument))
                        arguments_vector.push_back(nb::cast<double>(argument[i]));
                    else{
                        auto array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(argument);
                        arguments_vector.push_back(array(i));
                    }
                }
                results[i] = func(arguments_vector);
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

inline bool check_int_dtype(const nb::object& array){
    if(nb::isinstance<nb::ndarray<const int>>(array)
       || nb::isinstance<nb::ndarray<const unsigned int>>(array)
       || nb::isinstance<nb::ndarray<const long>>(array)
       || nb::isinstance<nb::ndarray<const unsigned long>>(array)
            ){
        return true;
    }
    return false;
}

#endif // WRAPPER_TEMPLATE_H