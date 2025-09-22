#ifndef WRAPPER_MULTI_ARGUMENT_H
#define WRAPPER_MULTI_ARGUMENT_H

#include <nanobind/nanobind.h>

#include <vector>

#include "types.h"
#include "utils.h"

namespace nb = nanobind;

/**
 * Prepares a 1-D NumPy array filled with copies of a scalar value.
 * @param scalar  A Python scalar (int or float) to be broadcasted.
 * @param shape   The size of the resulting 1-D NumPy array.
 * @return        A 1-D nb::ndarray<double> where all elements are equal to `scalar`.
 */
inline nb::object prepare_array_argument(const nb::object& scalar, std::size_t shape) {
  double* results = new double[shape];
  for (std::size_t i = 0; i < shape; i++) {
    results[i] = nb::cast<double>(scalar);
  }

  nb::capsule owner(results, [](void* p) noexcept { delete[] (double*)p; });
  auto result_array = nb::ndarray<double, nb::numpy>(results, {shape}, owner).cast();
  return result_array;
}

/**
 * Wraps a multi-argument function to support vectorized or scalar inputs.
 * The function returns either a single scalar (if all inputs were scalars) or a
 * 1-D NumPy array of results corresponding to each set of arguments.
 *
 * @param func   The multi-argument function to wrap. Accepts a vector of
 *               std::variant<double, int> and returns a double.
 * @param input  Vector of nb::object representing the arguments (scalars, lists, or 1-D arrays).
 * @return       Either a scalar nb::object (if all inputs are scalars) or a 1-D
 *               nb::ndarray<double> containing results of `func` applied element-wise.
 *
 * @throws nb::type_error  If any input is not a float, int, list, or 1-D NumPy array.
 * @throws nb::value_error If lists/arrays have incompatible lengths.
 * @throws std::runtime_error For other errors during processing of 1-D arrays.
 */
inline nb::object wrap_multiargument_function(const MultiargumentFunc& func, const std::vector<nb::object>& input) {
  // Check for scalar types (float or int)
  bool scalars_only = true;
  size_t input_length = 0;  // length of input if it is not a scalar
  for (const auto& argument : input) {
    if (nb::isinstance<nb::list>(argument)) {
      auto list = nb::cast<nb::list>(argument);
      input_length = nb::len(list);
      scalars_only = false;
      break;
    } else if (nb::isinstance<nb::ndarray<>>(argument)) {
      auto array = nb::cast<nb::ndarray<>>(argument);
      input_length = array.size();
      scalars_only = false;
      break;
    } else if (!nb::isinstance<nb::float_>(argument) && !nb::isinstance<nb::int_>(argument)) {
      // Handle unsupported types
      throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
    }
  }
  if (scalars_only) {
    // there is no array or list argument
    std::vector<std::variant<double, int>> input_casted;
    input_casted.reserve(input.size());
    for (const auto& argument : input) {
      if (PyFloat_Check(argument.ptr()))
        input_casted.emplace_back(nb::cast<double>(argument));
      else {
        input_casted.emplace_back(nb::cast<int>(argument));
      }
    }
    double result = func(input_casted);
    return nb::cast(result);
  }
  // Check for Python list and / or arrays
  else {
    // there is at least one list or array argument. input_length is length of the first one, others should have the
    // same shape.
    std::vector<nb::object> arguments;
    for (const auto& input_element : input) {
      if (nb::isinstance<nb::float_>(input_element) || nb::isinstance<nb::int_>(input_element))
        arguments.push_back(prepare_array_argument(input_element, (int)input_length));
      else if (nb::isinstance<nb::list>(input_element)) {
        arguments.push_back(input_element);
        if (nb::len(nb::cast<nb::list>(input_element)) != input_length)
          throw nb::value_error("Incompatible lists/arrays size");
      } else if (nb::isinstance<nb::ndarray<>>(input_element)) {
        // First, cast to a generic ndarray to check ndim and size
        auto array_generic = nb::cast<nb::ndarray<>>(input_element);
        if (array_generic.ndim() != 1) throw nb::value_error("Input NumPy array must be 1-D.");

        // Now safely cast to the required type
        auto array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(input_element);
        arguments.push_back(input_element);
        if (array.size() != input_length) throw nb::value_error("Incompatible lists/arrays size");
      } else {
        // Handle unsupported types
        throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
      }
    }

    double* results = new double[input_length];
    try {
      for (size_t i = 0; i < input_length; i++) {
        std::vector<std::variant<double, int>> arguments_vector;
        for (auto& argument : arguments) {
          if (nb::isinstance<nb::list>(argument)) {
            if (PyFloat_Check(argument[i].ptr()))
              arguments_vector.emplace_back(nb::cast<double>(argument[i]));
            else {
              arguments_vector.emplace_back(nb::cast<int>(argument[i]));
            }
          } else {
            auto array = nb::cast<nb::ndarray<const double, nb::shape<-1>>>(argument);
            arguments_vector.emplace_back(array(i));
          }
        }
        results[i] = func(arguments_vector);
      }
    } catch (const nb::cast_error& e) {
      throw nb::type_error("1-D NumPy array dtype cannot be cast to double or input is not suitable.");
    } catch (const std::exception& e) {
      throw std::runtime_error("Error processing 1-D NumPy array: " + std::string(e.what()));
    }

    nb::capsule owner(results, [](void* p) noexcept { delete[] (double*)p; });

    auto result_array = nb::ndarray<double, nb::numpy>(results, {input_length}, owner).cast();
    return result_array;
  }
}

#endif
