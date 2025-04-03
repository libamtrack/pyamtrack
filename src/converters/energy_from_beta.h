#ifndef ENERGY_FROM_BETA_H
#define ENERGY_FROM_BETA_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

const char* energy_from_beta_doc = R"pbdoc(
    Calculate energy per nucleon (MeV/u) from beta.

    Parameters:
        input (float | int | numpy.ndarray | list): The beta value(s). Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated energy value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
    )pbdoc";

py::object PYBIND11_EXPORT energy_from_beta(py::object input);

#endif // ENERGY_FROM_BETA_H