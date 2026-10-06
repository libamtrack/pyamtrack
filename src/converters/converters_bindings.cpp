#include <nanobind/nanobind.h>

#include "beta_from_energy.h"
#include "dose_from_fluence.h"
#include "energy_from_beta.h"
#include "energy_from_energy_per_amu.h"
#include "energy_from_gamma.h"
#include "energy_from_momentum.h"
#include "fluence_from_dose.h"
#include "gamma_from_energy.h"
#include "momentum_from_energy.h"

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

const char* dose_from_fluence_doc = R"pbdoc(
    Calculate dose in Gy for particle with given fluence and energy

    TODO
)pbdoc";

const char* fluence_from_dose_doc = R"pbdoc(
    Calculate fluence in 1/cm2 for particles with given dose and energy

    TODO
)pbdoc";

const char* energy_from_energy_per_amu_doc = R"pbdoc(
    Calculate energy per nucleon from kinetic energy of particle
)pbdoc";

NB_MODULE(converters, m) {
  m.doc() = "Functions for converting between different physical quantities.";

  m.def("beta_from_energy", &beta_from_energy, nb::arg("energy_MeV_u"), beta_from_energy_doc);
  m.def("gamma_from_energy", &gamma_from_energy, nb::arg("energy_MeV_u"), gamma_from_energy_doc);

  m.def("energy_from_beta", &energy_from_beta, nb::arg("beta"), energy_from_beta_doc);

  m.def("energy_from_gamma", &energy_from_gamma, nb::arg("gamma"), energy_from_gamma_doc);

  m.def("momentum_from_energy", &momentum_from_energy, nb::arg("energy_MeV_u"), momentum_from_energy_doc);

  m.def("energy_from_momentum", &energy_from_momentum, nb::arg("momentum"), energy_from_momentum_doc);

  m.def("dose_from_fluence", &dose_from_fluence, nb::arg("energy_MeV_u"), nb::arg("particle"), nb::arg("fluence_cm2"),
        nb::arg("material") = 1, nb::arg("stopping_power_source") = 1, nb::arg("cartesian_product") = false,
        dose_from_fluence_doc);

  m.def("fluence_from_dose", &fluence_from_dose, nb::arg("energy_MeV_u"), nb::arg("particle"), nb::arg("dose"),
        nb::arg("material") = 1, nb::arg("stopping_power_source") = 1, nb::arg("cartesian_product") = false,
        dose_from_fluence_doc);

  m.def("energy_from_energy_per_amu", &energy_from_energy_per_amu, nb::arg("energy_MeV_u"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_from_energy_per_amu_doc);
}
