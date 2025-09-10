#ifndef WRAPPER_TEMPLATE_H
#define WRAPPER_TEMPLATE_H

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/vector.h>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace nb = nanobind;

// Define the type for the function to be wrapped.
using Func = std::function<double(double)>;
using MultiargumentFunc = std::function<double(const std::vector<std::variant<double, int>>&)>;
inline bool check_int_dtype(const nb::object& array) {
  if (nb::isinstance<nb::ndarray<int8_t>>(array) || nb::isinstance<nb::ndarray<uint8_t>>(array) ||
      nb::isinstance<nb::ndarray<int16_t>>(array) || nb::isinstance<nb::ndarray<uint16_t>>(array) ||
      nb::isinstance<nb::ndarray<int32_t>>(array) || nb::isinstance<nb::ndarray<uint32_t>>(array) ||
      nb::isinstance<nb::ndarray<int64_t>>(array) || nb::isinstance<nb::ndarray<uint64_t>>(array)) {
    return true;
  }
  return false;
}
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
      for (size_t i = 0; i < input_array.ndim(); ++i) result_shape[i] = input_array.shape(i);

      // Get the pointer to the block of data to be mapped
      const double* data_buffer = input_array.data();

      // Initialize the vector that will store the result
      // And map all the elements from the input with the given func
      std::vector<double> results(num_elements);
      for (size_t i = 0; i < num_elements; ++i) {
        results[i] = func(data_buffer[i]);
      }

      // Create the result ndarray, with the mapped data and pass the according shape
      auto result_array =
          nb::ndarray<double, nb::numpy>(results.data(), result_shape.size(), result_shape.data()).cast();
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

inline nb::object prepare_array_argument(const nb::object& scalar, std::size_t shape) {
  std::vector<double> results(shape);
  for (std::size_t i = 0; i < shape; i++) {
    results[i] = nb::cast<double>(scalar);
  }
  auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {static_cast<std::size_t>(shape)}).cast();
  return result_array;
}

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
    return nb::cast(static_cast<double>(result));
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
    std::vector<double> results(input_length);
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

    auto result_array = nb::ndarray<double, nb::numpy>(results.data(), {input_length}).cast();
    return result_array;
  }
}
/**
 * This function applies a multi-argument function to the cartesian product of input argument lists or arrays.
 * For each argument in the input vector, if it is a list or ndarray, it is expanded into its elements.
 * The function then computes the cartesian product of all such argument sets, and applies the provided
 * MultiargumentFunc to each combination, collecting the results in a nested list structure.
 *
 * @param func   The multi-argument function to apply. It should accept a vector of std::variant<double, int>.
 * @param input  A vector of nanobind objects, each representing an argument. Each argument can be a scalar,
 *               list, or ndarray. Lists and ndarrays are expanded; scalars are treated as single values.
 * @return       A nested nanobind list (nb::object) containing the results of applying func to each
 *               combination of arguments from the cartesian product.
 *
 * Differs from wrap_multiargument_function in that this function computes the cartesian product of argument
 * lists/arrays, applying the function to every possible combination, whereas wrap_multiargument_function
 * applies the function to a single set of arguments (possibly vectorized).
 */
inline nb::object wrap_cartesian_product_function(const MultiargumentFunc& func, const std::vector<nb::object>& input) {
  // Check for scalar types (float or int)
  std::vector<size_t> shape;
  std::vector<std::vector<nb::object>> expanded_inputs;
  expanded_inputs.reserve(input.size());

  for (const auto& argument : input) {
    if (nb::isinstance<nb::list>(argument)) {
      auto list = nb::cast<nb::list>(argument);
      shape.push_back(nb::len(list));
      std::vector<nb::object> tmp;
      tmp.reserve(nb::len(list));
      for (auto item : list) tmp.push_back(nb::cast(item));
      expanded_inputs.push_back(std::move(tmp));
    } else if (nb::isinstance<nb::ndarray<>>(argument)) {
      if (check_int_dtype(argument)) {
        auto array = nb::cast<nb::ndarray<long long, nb::shape<-1>>>(argument);

        shape.push_back(array.size());
        std::vector<nb::object> tmp;
        tmp.reserve(array.size());
        for (size_t i = 0; i < array.size(); ++i) {
          tmp.push_back(nb::cast(array(i)));
        }
        expanded_inputs.push_back(std::move(tmp));
      } else {
        auto array = nb::cast<nb::ndarray<double, nb::shape<-1>>>(argument);
        shape.push_back(array.size());
        std::vector<nb::object> tmp;
        tmp.reserve(array.size());
        for (size_t i = 0; i < array.size(); ++i) {
          tmp.push_back(nb::cast(array(i)));
        }
        expanded_inputs.push_back(std::move(tmp));
      }
    } else if (nb::isinstance<nb::float_>(argument) || nb::isinstance<nb::int_>(argument)) {
      shape.push_back(1);
      expanded_inputs.push_back({argument});
    } else {
      // Handle unsupported types
      throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
    }
  }

  std::function<nb::object(size_t, std::vector<nb::object>&)> build;
  build = [&](size_t depth, std::vector<nb::object>& current) -> nb::object {
    if (depth == expanded_inputs.size()) {
      return wrap_multiargument_function(func, current);
    }
    nb::list out;
    for (auto& val : expanded_inputs[depth]) {
      current.push_back(val);
      out.append(build(depth + 1, current));
      current.pop_back();
    }
    return out;
  };
  std::vector<nb::object> current;
  return build(0, current);
}

/* This C++ function template performs a type-safe conversion of the value held inside a std::variant<double, int>
 * to another type T using std::visit. */
template <typename T>
T variant_cast(const std::variant<double, int>& v) {
  return std::visit([](auto&& arg) -> T { return static_cast<T>(arg); }, v);
}

#endif  // WRAPPER_TEMPLATE_H
