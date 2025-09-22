#ifndef WRAPPER_UTILS_H
#define WRAPPER_UTILS_H

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include <variant>

namespace nb = nanobind;

// Check if a given NumPy array has an integer dtype.
// Returns true if the array's dtype is any of the standard integer types (signed or unsigned, 8/16/32/64-bit).
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
  for (int i = arr.ndim() - 1; i >= 0; --i) {
    if (arr.stride(i) != expected_stride) return false;
    expected_stride *= arr.shape(i);
  }
  return true;
}

// Safely converts the value stored in a std::variant<double, int> to type T.
// Uses std::visit to handle both double and int cases and casts the value to the requested type.
template <typename T>
T variant_cast(const std::variant<double, int>& v) {
  return std::visit([](auto&& arg) -> T { return static_cast<T>(arg); }, v);
}

#endif
