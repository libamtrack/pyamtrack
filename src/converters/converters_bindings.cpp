#include <nanobind/nanobind.h>

#include "beta_from_energy.h"
#include "energy_from_beta.h"
#include "gamma_from_energy.h"
#include "energy_from_gamma.h"
#include "momentum_from_energy.h"
#include "energy_from_momentum.h"

namespace nb = nanobind;

const char* beta_from_energy_doc = R"pbdoc(
    Calculate beta from energy per nucleon (MeV/u).

    Parameters:
        energy_MeV_u (float | int | numpy.ndarray | list): The particle kinetic energy in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated beta value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

const char* energy_from_beta_doc = R"pbdoc(
    Calculate energy per nucleon (MeV/u) from beta.

    Parameters:
        beta (float | int | numpy.ndarray | list): The beta value(s). Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated energy value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
    )pbdoc";

const char* gamma_from_energy_doc = R"pbdoc(
    Calculate relativistic gamma for single value of energy (MeV/u).

    Parameters:
        energy_MeV_u (float | int | numpy.ndarray | list): The particle kinetic energy in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated gamma value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

const char* energy_from_gamma_doc = R"pbdoc(
    Calculate energy for single value of relativistic gamma

    Parameters:
        gamma (float | int | numpy.ndarray | list): The relativistic gamma value(s). Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated energy value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

const char* momentum_from_energy_doc = R"pbdoc(
    Calculate relativistic momentum (per nucleon) of particle

    Parameters:
        energy_MeV_u (float | int | numpy.ndarray | list): The particle kinetic energy in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated gamma value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";

const char* energy_from_momentum_doc = R"pbdoc(
    Calculate energy per nucleon of particle with given momentum per nucleon

    Parameters:
        momentum (float | int | numpy.ndarray | list): The particle kinetic energy in MeV/u. Can be a single value, a NumPy array, or a Python list.

    Returns:
        float | numpy.ndarray | list: The calculated gamma value(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
)pbdoc";


NB_MODULE(converters, m) {
  m.doc() = "Functions for converting between different physical quantities.";

  m.def("beta_from_energy", &beta_from_energy, nb::arg("energy_MeV_u"), beta_from_energy_doc);
  m.def("gamma_from_energy", &gamma_from_energy, nb::arg("energy_MeV_u"), gamma_from_energy_doc);

  m.def("energy_from_beta", &energy_from_beta, nb::arg("beta"), energy_from_beta_doc);

  m.def("energy_from_gamma", &energy_from_gamma, nb::arg("gamma"), energy_from_gamma_doc);
  
  m.def("momentum_from_energy", &momentum_from_energy, nb::arg("energy_MeV_u"), momentum_from_energy_doc );

  m.def("energy_from_momentum", &energy_from_momentum, nb::arg("momentum"), energy_from_momentum_doc);
}
