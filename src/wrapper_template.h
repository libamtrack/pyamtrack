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
  return nb::isinstance<nb::ndarray<int8_t>>(array) || nb::isinstance<nb::ndarray<uint8_t>>(array) ||
         nb::isinstance<nb::ndarray<int16_t>>(array) || nb::isinstance<nb::ndarray<uint16_t>>(array) ||
         nb::isinstance<nb::ndarray<int32_t>>(array) || nb::isinstance<nb::ndarray<uint32_t>>(array) ||
         nb::isinstance<nb::ndarray<int64_t>>(array) || nb::isinstance<nb::ndarray<uint64_t>>(array);
}

// Function to check whether given ndarray is C-contiguous (row-major contiguous).
//
// C-contiguous means the array is stored in memory row by row (like in C),
// opposite to to Fortran’s column-major format.
//
// Ndarrays can become non-contiguous, for example after a slicing operation
// like array[::2], where the elements are no longer stored in one continuous block.
//
// To keep track of where each element lives in memory, NumPy uses strides.
// A stride tells you how many steps in memory you must move
// to advance by 1 element along a given axis.
//   - In NumPy’s Python API, strides are expressed in bytes.
//   - In nanobind, strides are expressed in number of elements
//
// Example: for a 2D array of shape (3, 4):
//   stride[1] = 1      -> moving along columns steps through consecutive elements
//   stride[0] = 4      -> moving to the next row skips over 4 elements
//
// For an array to be C-contiguous, the last stride must always be 1,
// and each preceding stride must equal the product of all dimensions after it.
template <typename T>
inline bool is_c_contiguous(const nb::ndarray<T>& arr) {
  if (arr.size() == 0) return true;
  size_t expected_stride = 1;
  for (ssize_t i = arr.ndim() - 1; i >= 0; --i) {
    if (arr.stride(i) != expected_stride) return false;
    expected_stride *= arr.shape(i);
  }
  return true;
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

      if (!is_c_contiguous(input_array)) {
        throw nb::value_error(
            "NDArray must be C-contiguous. "
            "Use numpy.ascontiguousarray(your_array) before passing it.");
      }

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
      double* results = new double[num_elements];
      for (size_t i = 0; i < num_elements; ++i) {
        results[i] = func(data_buffer[i]);
      }

      // Create the result ndarray, with the mapped data and pass the according shape
      nb::capsule owner(results, [](void* p) noexcept { delete[] (double*)p; });
      auto result_array =
          nb::ndarray<double, nb::numpy>(results, result_shape.size(), result_shape.data(), owner).cast();
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
  double* results = new double[shape];
  for (std::size_t i = 0; i < shape; i++) {
    results[i] = nb::cast<double>(scalar);
  }

  nb::capsule owner(results, [](void* p) noexcept { delete[] (double*)p; });
  auto result_array = nb::ndarray<double, nb::numpy>(results, {static_cast<std::size_t>(shape)}, owner).cast();
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

/**
 * Flattens a NumPy array (ndarray) or scalar into a 1-dimensional vector of nb::object.
 * Records the original shape of the array in output_shape, except for 0-dimensional arrays.
 * Ensures the array is C-contiguous (row-major); raises a value_error otherwise.
 *
 * @tparam T             The element type of the ndarray.
 * @param argument       A nanobind handle representing the input ndarray or scalar.
 * @param array_inputs   Vector of vectors to which the flattened array is appended.
 * @param output_shape   Vector to record the shape of the input array.
 */
template <typename T>
inline void process_array(nb::handle argument, std::vector<std::vector<nb::object>>& array_inputs,
                          std::vector<size_t>& output_shape) {
  auto arr = nb::cast<nb::ndarray<T>>(argument);
  if (!is_c_contiguous(arr)) {
    throw nb::value_error(
        "NDArray must be C-contiguous. "
        "Use numpy.ascontiguousarray(your_array) before passing it.");
  }

  if (arr.ndim() == 0) {
    // scalar ndarray
    T* ptr = arr.data();
    array_inputs.push_back({nb::cast(ptr[0])});
  } else {
    // flatten NDarray and record its original shape
    std::vector<nb::object> tmp;
    tmp.reserve(arr.size());

    for (size_t i = 0; i < arr.ndim(); ++i) {
      output_shape.push_back(arr.shape(i));
    }

    T* ptr = arr.data();
    for (size_t i = 0; i < arr.size(); ++i) {
      tmp.push_back(nb::cast(ptr[i]));
    }
    array_inputs.push_back(std::move(tmp));
  }
}

// Parse the input consisting of lists, arrays and scalars and unify their representation
inline std::pair<std::vector<std::vector<nb::object>>, std::vector<size_t>> parse_input(
    const std::vector<nb::object>& input) {
  std::vector<std::vector<nb::object>> array_inputs;
  std::vector<size_t> output_shape;

  array_inputs.reserve(input.size());

  for (const auto& argument : input) {
    // 1. Check for list
    if (nb::isinstance<nb::list>(argument)) {
      auto list = nb::cast<nb::list>(argument);
      std::vector<nb::object> tmp;
      tmp.reserve(nb::len(list));
      for (auto item : list) tmp.push_back(nb::cast(item));
      array_inputs.push_back(std::move(tmp));

      output_shape.push_back(nb::len(list));
    }
    // 2. Check for ndarray
    else if (nb::isinstance<nb::ndarray<>>(argument)) {
      if (check_int_dtype(argument)) {
        process_array<long long>(argument, array_inputs, output_shape);
      } else {
        process_array<double>(argument, array_inputs, output_shape);
      }

      // 3. Check for scalar
    } else if (nb::isinstance<nb::float_>(argument) || nb::isinstance<nb::int_>(argument)) {
      array_inputs.push_back({argument});
    } else {
      // Handle unsupported types
      throw nb::type_error("Input must be a float, int, list, or 0-D/1-D NumPy array.");
    }
  }
  return {array_inputs, output_shape};
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
  // Parse the input object
  auto [array_inputs, output_shape] = parse_input(input);

  // Record the size of every input, return empty np.array in case any of the inputs is empty
  std::vector<size_t> shape;

  for (const auto& input_vector : array_inputs) {
    ssize_t size = input_vector.size();
    if (size <= 0) {
      return nb::ndarray<double, nb::numpy>(nullptr, {0}).cast();
    }
    shape.push_back(size);
  }

  // Initialize the vector of pointers and compute the number of elements in the output
  int num_inputs = shape.size();
  std::vector<size_t> index_pointers(num_inputs, 0);
  int output_size = 1;

  for (size_t i = 0; i < num_inputs; ++i) {
    output_size *= shape[i];
  }

  // Iterate through all the combinations and fill the array with functions output
  double* results = new double[output_size];
  std::vector<std::variant<double, int>> args(num_inputs);

  auto assign_val = [&](nb::handle val, int j) {
    if (nb::isinstance<nb::float_>(val)) {
      args[j] = nb::cast<double>(val);
    } else if (nb::isinstance<nb::int_>(val)) {
      args[j] = nb::cast<int>(val);
    } else {
      throw nb::type_error("All arguments must be int or float at the deepest level.");
    }
  };

  for (int j = 0; j < num_inputs; ++j) {
    auto val = array_inputs[j][index_pointers[j]];
    assign_val(val, j);
  }

  results[0] = func(args);

  for (size_t i = 1; i < output_size; ++i) {
    int j = num_inputs - 1;

    while (index_pointers[j] >= shape[j] - 1) {
      index_pointers[j] = 0;
      auto val = array_inputs[j][index_pointers[j]];
      assign_val(val, j);
      --j;
    }

    index_pointers[j] += 1;
    auto val = array_inputs[j][index_pointers[j]];
    assign_val(val, j);

    results[i] = func(args);
  }

  // Transform the raw pointer and return nb::ndarray
  nb::capsule owner(results, [](void* p) noexcept { delete[] (double*)p; });

  auto result_array =
      nb::ndarray<double, nb::numpy>(results, output_shape.size(), output_shape.data(), nb::handle(owner)).cast();
  return result_array;
}

/* This C++ function template performs a type-safe conversion of the value held inside a std::variant<double, int>
 * to another type T using std::visit. */
template <typename T>
T variant_cast(const std::variant<double, int>& v) {
  return std::visit([](auto&& arg) -> T { return static_cast<T>(arg); }, v);
}

#endif  // WRAPPER_TEMPLATE_H
