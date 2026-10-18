#include <nanobind/nanobind.h>

#include "beta_from_energy.h"
#include "dose_from_fluence.h"
#include "energy_from_beta.h"
#include "energy_from_energy_per_amu.h"
#include "energy_from_gamma.h"
#include "energy_from_momentum.h"
#include "energy_per_amu_from_energy.h"
#include "fluence_from_dose.h"
#include "gamma_from_energy.h"
#include "momentum_from_energy.h"

namespace nb = nanobind;

const char* beta_from_energy_doc = R"pbdoc(
    Calculate the relativistic beta from the total kinetic energy of a particle.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy of the particle in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles used to convert the total energy to energy per
        nucleon. A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Relativistic beta value(s). The result is a scalar for scalar inputs
        and a NumPy array when at least one input is array-like.

    Notes
    -----
    When ``cartesian_product=False``, array-like arguments must have compatible
    lengths. When ``cartesian_product=True``, the result contains all
    combinations of the provided array-like arguments.
)pbdoc";


const char* energy_from_beta_doc = R"pbdoc(
    Calculate the particle kinetic energy from the relativistic beta.

    Parameters
    ----------
    beta : float, int, list, or numpy.ndarray
        Relativistic beta value(s).

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles for which the kinetic energy is calculated.
        A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Kinetic energy value(s) in MeV. The result is a scalar for scalar
        inputs and a NumPy array when at least one input is array-like.

    Notes
    -----
    When ``cartesian_product=False``, array-like arguments must have compatible
    lengths. When ``cartesian_product=True``, the result contains all
    combinations of the provided array-like arguments.
)pbdoc";


const char* gamma_from_energy_doc = R"pbdoc(
    Calculate the relativistic Lorentz factor gamma from the total kinetic
    energy of a particle.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy of the particle in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles used to convert the total energy to energy per
        nucleon. A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Relativistic gamma value(s). The result is a scalar for scalar inputs
        and a NumPy array when at least one input is array-like.
)pbdoc";


const char* energy_from_gamma_doc = R"pbdoc(
    Calculate the kinetic energy of a particle from the relativistic gamma.

    Parameters
    ----------
    gamma : float, int, list, or numpy.ndarray
        Relativistic gamma value(s).

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles for which the kinetic energy is calculated.
        A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Kinetic energy value(s) in MeV. The result is a scalar for scalar
        inputs and a NumPy array when at least one input is array-like.
)pbdoc";


const char* momentum_from_energy_doc = R"pbdoc(
    Calculate the relativistic momentum per nucleon from the total kinetic
    energy of a particle.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy of the particle in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles used to convert the total energy to energy per
        nucleon. A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Relativistic momentum per nucleon in MeV/c/u. The result is a scalar
        for scalar inputs and a NumPy array when at least one input is
        array-like.
)pbdoc";


const char* energy_from_momentum_doc = R"pbdoc(
    Calculate the kinetic energy of a particle from its relativistic momentum
    per nucleon.

    Parameters
    ----------
    momentum : float, int, list, or numpy.ndarray
        Relativistic momentum per nucleon in MeV/c/u.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles for which the kinetic energy is calculated.
        A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Kinetic energy value(s) in MeV. The result is a scalar for scalar
        inputs and a NumPy array when at least one input is array-like.
)pbdoc";


const char* dose_from_fluence_doc = R"pbdoc(
    Calculate absorbed dose from particle fluence.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy of the particle in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles depositing the dose. A particle is represented
        by a pyamtrack Ion object.

    fluence_cm2 : float, int, list, or numpy.ndarray
        Particle fluence in particles per cm^2.

    material : int or Material, list, or numpy.ndarray, optional
        Target material. The default is material ID 1.

    stopping_power_source : int, StoppingPowerSource, or str, optional
        Stopping-power data source. Supported values are:

        - ``0`` or ``"default"``
        - ``1`` or ``"bethe"``
        - ``2`` or ``"pstar"``
        - ``3`` or ``"icru"``

        The default is ``1`` (Bethe).

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Absorbed dose in Gy. The result is a scalar for scalar inputs and a
        NumPy array when at least one input is array-like.
)pbdoc";


const char* fluence_from_dose_doc = R"pbdoc(
    Calculate particle fluence required to deliver a specified absorbed dose.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy of the particle in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles delivering the dose. A particle is represented
        by a pyamtrack Ion object.

    dose : float, int, list, or numpy.ndarray
        Absorbed dose in Gy.

    material : int or Material, list, or numpy.ndarray, optional
        Target material. The default is material ID 1.

    stopping_power_source : int, StoppingPowerSource, or str, optional
        Stopping-power data source. Supported values are:

        - ``0`` or ``"default"``
        - ``1`` or ``"bethe"``
        - ``2`` or ``"pstar"``
        - ``3`` or ``"icru"``

        The default is ``1`` (Bethe).

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Required particle fluence in particles per cm^2. The result is a
        scalar for scalar inputs and a NumPy array when at least one input is
        array-like.
)pbdoc";


const char* energy_from_energy_per_amu_doc = R"pbdoc(
    Calculate the total kinetic energy of a particle from its kinetic energy
    per nucleon.

    Parameters
    ----------
    energy_MeV_u : float, int, list, or numpy.ndarray
        Kinetic energy per nucleon in MeV/u.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles for which the total kinetic energy is
        calculated. A particle is represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Total kinetic energy value(s) in MeV.
)pbdoc";


const char* energy_per_amu_from_energy_doc = R"pbdoc(
    Calculate the kinetic energy per nucleon of a particle from its total
    kinetic energy.

    Parameters
    ----------
    energy_MeV : float, int, list, or numpy.ndarray
        Total kinetic energy in MeV.

    particle : Ion, list[Ion], or numpy.ndarray
        Particle or particles used for the conversion. A particle is
        represented by a pyamtrack Ion object.

    cartesian_product : bool, optional
        If False, array arguments are evaluated elementwise. If True, all
        combinations of array-like arguments are evaluated. Default: False.

    Returns
    -------
    float or numpy.ndarray
        Kinetic energy per nucleon value(s) in MeV/u.
)pbdoc";

NB_MODULE(converters, m) {
  m.doc() = "Functions for converting between different physical quantities.";

  m.def("beta_from_energy", &beta_from_energy, nb::arg("energy_MeV"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, beta_from_energy_doc);

  m.def("gamma_from_energy", &gamma_from_energy, nb::arg("energy_MeV"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, gamma_from_energy_doc);

  m.def("energy_from_beta", &energy_from_beta, nb::arg("beta"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_from_beta_doc);

  m.def("energy_from_gamma", &energy_from_gamma, nb::arg("gamma"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_from_gamma_doc);

  m.def("momentum_from_energy", &momentum_from_energy, nb::arg("energy_MeV_u"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, momentum_from_energy_doc);

  m.def("energy_from_momentum", &energy_from_momentum, nb::arg("momentum"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_from_momentum_doc);

  m.def("dose_from_fluence", &dose_from_fluence, nb::arg("energy_MeV"), nb::arg("particle"), nb::arg("fluence_cm2"),
        nb::arg("material") = 1, nb::arg("stopping_power_source") = 1, nb::arg("cartesian_product") = false,
        dose_from_fluence_doc);

  m.def("fluence_from_dose", &fluence_from_dose, nb::arg("energy_MeV"), nb::arg("particle"), nb::arg("dose"),
        nb::arg("material") = 1, nb::arg("stopping_power_source") = 1, nb::arg("cartesian_product") = false,
        dose_from_fluence_doc);

  m.def("energy_from_energy_per_amu", &energy_from_energy_per_amu, nb::arg("energy_MeV_u"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_from_energy_per_amu_doc);

  m.def("energy_per_amu_from_energy", &energy_per_amu_from_energy, nb::arg("energy_MeV"), nb::arg("particle"),
        nb::arg("cartesian_product") = false, energy_per_amu_from_energy_doc);
}
