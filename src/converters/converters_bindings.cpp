#include <pybind11/pybind11.h>
#include "beta_from_energy.h"
#include "energy_from_beta.h"

namespace py = pybind11;

const char* beta_from_energy_doc = R"pbdoc(
    Calculate beta from energy per nucleon (MeV/u).

    Parameters:
        input (float | int | numpy.ndarray | list): The energy per nucleon in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated beta value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

const char* energy_from_beta_doc = R"pbdoc(
    Calculate energy per nucleon (MeV/u) from beta.

    Parameters:
        input (float | int | numpy.ndarray | list): The beta value(s). Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated energy value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
    )pbdoc";

PYBIND11_MODULE(converters, m) {
    m.doc() = "Functions for converting between different physical quantities.";

    m.def(
        "beta_from_energy",
        &beta_from_energy,
        py::arg("input"),
        py::return_value_policy::automatic,
        beta_from_energy_doc
    );

    m.def(
        "energy_from_beta",
        &energy_from_beta,
        py::arg("input"),
        py::return_value_policy::automatic,
        energy_from_beta_doc
    );


}