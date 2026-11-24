#ifndef PARTICLE_ARGUMENTS_H
#define PARTICLE_ARGUMENTS_H

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include <stdexcept>
#include <string>

#include "ions/ion.h"
#include "particles.h"

namespace nb = nanobind;

/**
 * @brief Validates a particle argument, recursing into lists and NumPy arrays.
 *
 * Stopping-power calculations require an ion (Z and A). Elementary particles
 * such as neutron and electron are rejected. Integer particle numbers are not
 * accepted.
 *
 * @throws std::invalid_argument (ValueError) if an Ion is missing Z/A.
 * @throws nb::type_error if the argument is not an Ion, list, or NumPy array.
 */
inline void validate_particle_argument(const nb::object& argument) {
  if (nb::isinstance<nb::list>(argument)) {
    nb::list values = nb::cast<nb::list>(argument);
    for (size_t i = 0; i < values.size(); ++i) {
      validate_particle_argument(values[i]);
    }
    return;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    validate_particle_argument(argument.attr("tolist")());
    return;
  }

  if (nb::isinstance<Ion>(argument)) {
    nb::cast<Ion>(argument).get_internal_particle_no();
    return;
  }

  if (nb::isinstance<Particle>(argument)) {
    throw nb::type_error(
        "particle must be an Ion (stopping power requires Z and A); "
        "elementary particles are not supported");
  }

  throw nb::type_error("particle must be an Ion, list, or NumPy array of Ions");
}

/**
 * @brief Convert a particle argument to libamtrack particle number(s).
 *
 * Ion objects become 1000*Z + A. Lists and NumPy arrays are converted
 * elementwise.
 */
inline nb::object parse_particle_argument(const nb::object& argument) {
  if (nb::isinstance<Ion>(argument)) {
    return nb::cast(nb::cast<Ion>(argument).get_internal_particle_no());
  }

  if (nb::isinstance<Particle>(argument)) {
    throw nb::type_error(
        "particle must be an Ion (stopping power requires Z and A); "
        "elementary particles are not supported");
  }

  if (PyBool_Check(argument.ptr())) {
    throw nb::type_error("particle must be an Ion, list, or NumPy array of Ions");
  }

  if (nb::isinstance<nb::list>(argument)) {
    nb::list values = nb::cast<nb::list>(argument);
    nb::list parsed_values;
    for (size_t i = 0; i < values.size(); ++i) {
      parsed_values.append(parse_particle_argument(values[i]));
    }
    return parsed_values;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    return parse_particle_argument(argument.attr("tolist")());
  }

  throw nb::type_error("particle must be an Ion, list, or NumPy array of Ions");
}

#endif  // PARTICLE_ARGUMENTS_H
