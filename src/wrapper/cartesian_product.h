#ifndef WRAPPER_CARTESIAN_PRODUCT_H
#define WRAPPER_CARTESIAN_PRODUCT_H

#include <nanobind/nanobind.h>

#include <vector>

#include "types.h"
#include "utils.h"

namespace nb = nanobind;

/**
 * Flattens a NumPy array (ndarray) or scalar into a 1-dimensional vector of nb::object.
 * Records the original shape of the array in output_shape, except for 0-dimensional arrays.
 * Ensures the array is C-contiguous (row-major); raises a value_error otherwise.
 *
 * @tparam T             The element type of the ndarray.
 * @param argument       A nanobind handle representing the input ndarray or scalar.
 * @param array_inputs   Vector of vectors to which the flattened array is appended.
 * @param output_shape   Vector to record the shape of the input array.
 *
 * @throws nb::value_error if an input is not a non C-contigous Numpy array
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

/**
 * Parses a sequence of Python arguments (lists, ndarrays, or scalars) into a unified
 * internal representation for later cartesian product computation.
 *
 * @param input   Vector of nanobind objects, each representing a function argument
 *                (list, ndarray, or scalar).
 * @return        A pair consisting of:
 *                  - array_inputs: a vector of vectors of nb::object, each representing
 *                                  one argument’s expanded elements.
 *                  - output_shape: a vector of sizes (one per input) describing the
 *                                  number of elements from each argument.
 *
 * @throws nb::type_error if an input is not a float, int, list, or NumPy array.
 */
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
      throw nb::type_error("Input must be a float, int, list, or NumPy array.");
    }
  }
  return {array_inputs, output_shape};
}

/**
 * Applies a multi-argument function to the cartesian product of input arguments (each being lists or arrays).
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
 * @throws nb::type_error if input array contents are not integers or floats
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
    size_t size = input_vector.size();
    if (size == 0) {
      return nb::ndarray<double, nb::numpy>(nullptr, {0}).cast();
    }
    shape.push_back(size);
  }

  // Initialize the vector of pointers and compute the number of elements in the output
  size_t num_inputs = shape.size();
  std::vector<size_t> index_pointers(num_inputs, 0);
  size_t output_size = 1;

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

  for (size_t j = 0; j < num_inputs; ++j) {
    auto val = array_inputs[j][index_pointers[j]];
    assign_val(val, j);
  }

  results[0] = func(args);

  for (size_t i = 1; i < output_size; ++i) {
    size_t j = num_inputs - 1;

    while (j > 0 && index_pointers[j] >= shape[j] - 1) {
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
      nb::ndarray<double, nb::numpy>(results, output_shape.size(), output_shape.data(), owner).cast();
  return result_array;
}

#endif
