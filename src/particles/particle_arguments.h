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
 * such as neutron and electron are rejected. Integer values are treated as
 * libamtrack particle numbers (1000*Z + A). Boolean values are not accepted.
 *
 * @throws std::invalid_argument (ValueError) if a particle number does not
 *         correspond to a known element, or an Ion is missing Z/A.
 * @throws nb::type_error if the argument is not an int, Ion, list, or NumPy array.
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
    nb::cast<Ion>(argument).get_particle_no();
    return;
  }

  if (nb::isinstance<Particle>(argument)) {
    throw nb::type_error(
        "particle must be an Ion (stopping power requires Z and A); "
        "elementary particles are not supported");
  }

  long particle_no;
  if (PyBool_Check(argument.ptr())) {
    throw nb::type_error("particle must be an int (not bool), an Ion, or a list / NumPy array of either");
  } else if (nb::isinstance<nb::int_>(argument)) {
    particle_no = nb::cast<long>(argument);
  } else {
    throw nb::type_error("particle must be an int, an Ion, or a list / NumPy array of either");
  }

  const long Z = AT_Z_from_particle_no_single(particle_no);
  const long A = AT_A_from_particle_no_single(particle_no);
  if (A < 1) {
    throw std::invalid_argument("invalid particle number: " + std::to_string(particle_no));
  }
  bool known_Z = false;
  for (int i = 0; i < AT_Particle_Data.n; ++i) {
    if (AT_Particle_Data.Z[i] == Z) {
      known_Z = true;
      break;
    }
  }
  if (!known_Z) {
    throw std::invalid_argument("invalid particle number: " + std::to_string(particle_no));
  }
}

/**
 * @brief Convert a particle argument to libamtrack particle number(s).
 *
 * Ion objects become 1000*Z + A. Integers are passed through. Lists and
 * NumPy arrays are converted elementwise.
 */
inline nb::object parse_particle_argument(const nb::object& argument) {
  if (nb::isinstance<Ion>(argument)) {
    return nb::cast(nb::cast<Ion>(argument).get_particle_no());
  }

  if (nb::isinstance<Particle>(argument)) {
    throw nb::type_error(
        "particle must be an Ion (stopping power requires Z and A); "
        "elementary particles are not supported");
  }

  if (PyBool_Check(argument.ptr())) {
    throw nb::type_error("particle must be an integer (not bool), Ion, list, or NumPy array");
  }

  if (nb::isinstance<nb::int_>(argument)) {
    return argument;
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

  throw nb::type_error("particle must be an integer, Ion, list, or NumPy array");
}

#endif  // PARTICLE_ARGUMENTS_H
