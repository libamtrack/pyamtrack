#ifndef BETA_FROM_ENERGY_H
#define BETA_FROM_ENERGY_H

#include <pybind11/pybind11.h>

const char* beta_from_energy_doc = R"pbdoc(
    Calculate beta from energy per nucleon (MeV/u).

    Parameters:
        input (float | int | numpy.ndarray | list): The energy per nucleon in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated beta value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

pybind11::object PYBIND11_EXPORT beta_from_energy(pybind11::object input);

#endif // BETA_FROM_ENERGY_H