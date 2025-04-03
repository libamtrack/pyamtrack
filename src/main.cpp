#include <pybind11/pybind11.h>
#include "converters/beta_from_energy.h"
#include "converters/energy_from_beta.h"
#include "stopping/electron_range.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = "Python bindings for libamtrack";

    m.def(
        "beta_from_energy",
        &beta_from_energy,
        py::arg("input"),
        py::return_value_policy::automatic,
        R"pbdoc(
        Calculate beta from energy per nucleon (MeV/u).

        Parameters:
            input (float | int | numpy.ndarray | list): The energy per nucleon in MeV/u. Can be a single value, a NumPy array, or a Python list.

        Returns:
            float | numpy.ndarray | list: The calculated beta value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
        )pbdoc"
    );

    m.def(
        "energy_from_beta",
        &energy_from_beta,
        py::arg("input"),
        py::return_value_policy::automatic,
        R"pbdoc(
        Calculate energy per nucleon (MeV/u) from beta.

        Parameters:
            input (float | int | numpy.ndarray | list): The beta value(s). Can be a single value, a NumPy array, or a Python list.

        Returns:
            float | numpy.ndarray | list: The calculated energy value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
        )pbdoc"
    );

    m.def(
        "electron_range",
        &electron_range,
        py::arg("input"),
        py::return_value_policy::automatic,
        R"pbdoc(
        Calculate electron range (m).

        Parameters:
            input (float | int | numpy.ndarray | list): The energy per nucleon in MeV/u. Can be a single value, a NumPy array, or a Python list.

        Returns:
            float | numpy.ndarray | list: The calculated electron range(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
        )pbdoc"
    );

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}