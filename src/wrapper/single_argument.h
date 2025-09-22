#ifndef WRAPPER_SINGLE_ARGUMENT_H
#define WRAPPER_SINGLE_ARGUMENT_H

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/vector.h>

#include "types.h"
#include "utils.h"

namespace nb = nanobind;

/**
 * Wraps a single-argument C++ function to support scalar, Python list, or NumPy array inputs.
 *
 * @param func   A single-argument function of type `Func` (e.g., `double func(double)`).
 * @param input  A Python object representing the argument. Can be a float, int, list, or ndarray.
 * @return       The result of applying `func`:
 *                 - scalar nb::object if input is scalar
 *                 - nb::list if input is a Python list
 *                 - nb::ndarray<double> if input is a NumPy array
 *
 * @throws nb::type_error  If the input or list elements are not numeric, or if ndarray dtype cannot be cast to double.
 * @throws nb::value_error If a NumPy array is not C-contiguous.
 * @throws std::runtime_error For other errors during processing of NumPy arrays.
 */
inline nb::object wrap_function(Func func, const nb::object& input) {
  // 1. Check for scalar types (float or int)
  if (PyFloat_Check(input.ptr()) || PyLong_Check(input.ptr())) {
    double input_val = nb::cast<double>(input);
    double result = func(input_val);
    return nb::cast(result);
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

#endif
