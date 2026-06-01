#pragma once
#include <nanobind/nanobind.h>
#include "ion.h"

namespace nb = nanobind;

/**
 * @brief Initialize ion bindings in the Python module.
 *
 * @param ions Nanobind submodule for ions.
 */
void init_ions(nb::module_ &ions);

/**
 * @brief Create an Ion from atomic number Z and mass number A.
 *
 * @param Z Atomic number.
 * @param A Mass number.
 * @return Ion
 * @throws std::invalid_argument If the element or isotope is not found.
 */
Ion from_ZA(long long Z, long long A);