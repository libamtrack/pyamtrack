#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>
#include <vector>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <string> // Include for std::string

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
            results.push_back(static_cast<double>(func(item_val)));
        }
        return nb::cast(results);
    }
    // 3. Check for NumPy array
    else if (nb::isinstance<nb::ndarray<>>(input)) {
        // Get generic handle only for checking properties like ndim
        auto np_array_generic = nb::cast<nb::ndarray<>>(input);

        if (np_array_generic.ndim() > 1) {
            throw nb::value_error("Input NumPy array must be 0-D or 1-D.");
        }

        // Handle 0-D array (scalar)
        if (np_array_generic.ndim() == 0) {
            try {
                // FIX 1: Cast 'input' (the nb::object) directly to the desired typed ndarray
                auto np_array_typed = nb::cast<nb::ndarray<const double>>(input);
                double input_scalar = *np_array_typed.data(); // data() points to the scalar

                double result_scalar = static_cast<double>(func(input_scalar));

                // FIX 2: Use explicit constructor for allocation (0-D)
                auto result_array = nb::ndarray<nb::numpy, double>(
                    nullptr,    // data ptr (nullptr = allocate)
                    0,          // ndim
                    nullptr,    // shape pointer
                    nb::none()  // owner (let nanobind handle it)
                );
                *result_array.data() = result_scalar;
                return nb::cast(result_array);
            } catch (const nb::cast_error& e) {
                 throw nb::type_error("0-D NumPy array dtype cannot be cast to double.");
            } catch (const std::exception& e) {
                throw std::runtime_error("Error processing 0-D NumPy array: " + std::string(e.what()));
            }
        }

        // Handle 1-D array
        if (np_array_generic.ndim() == 1) {
             try {
                // Request read-only, C-contiguous double view/copy by casting 'input'
                auto input_array = nb::cast<nb::ndarray<const double, nb::c_contig>>(input);
                size_t size = input_array.shape(0);

                // FIX 2: Use explicit constructor for allocation (1-D)
                size_t shape_arr[] = {size}; // Create shape array on stack
                auto result_array = nb::ndarray<nb::numpy, double>(
                    nullptr,    // data ptr (nullptr = allocate)
                    1,          // ndim
                    shape_arr,  // shape pointer
                    nb::none()  // owner (let nanobind handle it)
                                // strides = nullptr (implies default C order)
                );

                const double* in_ptr = input_array.data();
                double* out_ptr = result_array.data();

                for (size_t i = 0; i < size; ++i) {
                    out_ptr[i] = static_cast<double>(func(in_ptr[i]));
                }
                return nb::cast(result_array);
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

#endif // WRAPPER_TEMPLATE_H