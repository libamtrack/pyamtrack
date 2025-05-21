#include <nanobind/nanobind.h>
#include "beta_from_energy.h"
#include "energy_from_beta.h"
#include "max_E_transfer_MeV.h"

namespace nb = nanobind;

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


const char* max_E_transfer_MeV_doc = R"pbdoc(
    "Calculate kinetic energy maximally transferred from an ion to an electron in a collision - relativistic or non-relativistic.

    Parameters:
        E_MeV_u (float | int | numpy.ndarray | list): energies of particle(s) per nucleon [MeV/u]; if positive, the computation will be relativistic; if negative, the classic formular will be used.
        A (float | int | numpy.ndarray | list): atomic mass of particle(s)

    Returns:
        float | numpy.ndarray | list: maximal energies transferred."
    )pbdoc";

NB_MODULE(converters, m) {
m.doc() = "Functions for converting between different physical quantities.";

m.def(
"beta_from_energy",
&beta_from_energy,
nb::arg("input"),
beta_from_energy_doc
);

m.def(
"energy_from_beta",
&energy_from_beta,
nb::arg("input"),
energy_from_beta_doc
);

m.def(
"max_E_transfer_MeV",
&max_E_transfer_MeV,
nb::arg("E_MeV_u"),
nb::arg("A"),
max_E_transfer_MeV_doc
);


}