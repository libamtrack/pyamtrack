#include <pybind11/pybind11.h>
#include "electron_range.h"

namespace py = pybind11;

PYBIND11_MODULE(stopping, m) {
    m.doc() = "Functions for calculating stopping power of ions and protons and range of particles in materials.";

    m.def(
        "electron_range",
        &electron_range,
        py::arg("input"),
        py::return_value_policy::automatic,
        R"pbdoc(
        Calculate electron range (m).

        Parameters:
            input (float | int | numpy.ndarray | list): The energy per nucleon in MeV. Can be a single value, a NumPy array, or a Python list.

        Returns:
            float | numpy.ndarray | list: The calculated electron range(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
        )pbdoc"
    );
}