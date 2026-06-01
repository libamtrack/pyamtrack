#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "AT_DataParticle.h"
#include "particles.h"
#include "construct_utils.h"
#include "ions/ions.h"
#include "ions/ion.h"

#include <iostream>

namespace nb = nanobind;

NB_MODULE(particles, m) {
  m.doc() = "Functions and data structures for accessing and manipulating particle properties.";

  nb::class_<Particle>(m, "Particle", R"pbdoc(
        Represents a particle with various physical properties.

        Prefer using particles.from_string() to construct instances.
        Pre-instantiated particles are available as module attributes
        (e.g., particles.proton, particles.C, particles.He).

        Attributes:
            id (int): Internal ID (row index in AT_Particle_Data).
            pdg (int): PDG code.
            atomic_weight (float): Atomic weight.
            element_name (str): Element name.
            element_acronym (str): Element acronym.
    )pbdoc")
      .def(nb::init<long long>(), R"pbdoc(
        Create a Particle from a PDG code.

        Notes:
            PDG codes corresponding to ions raise ValueError; use
            particles.ions.Ion(...) for ion PDG codes instead.

        Args:
            pdg_code (int): PDG code.

        Raises:
            ValueError: If the PDG code is invalid or corresponds to an ion.
      )pbdoc")
      
      .def("__str__", &Particle::str, "Return the element acronym (e.g., 'C').")
      .def("__repr__", &Particle::repr, "Return a detailed, debug-friendly representation.")
      .def_ro("id", &Particle::id, "The internal ID of the particle.")
      .def_ro("atomic_weight", &Particle::atomic_weight, "The atomic weight of the particle.")
      .def_ro("element_name", &Particle::element_name, "The name of the particle.")
      .def_ro("element_acronym", &Particle::element_acronym, "The acronym of the particle.")
      .def_ro("pdg", &Particle::pdg, "The PDG code of the particle.")
      ;

    m.def("get_names", &get_names, R"pbdoc(
      Get all particle names.

      Returns:
        list[str]: Element names in data order.
    )pbdoc");

    m.def("get_acronyms", &get_acronyms, R"pbdoc(
      Get all particle acronyms.

      Returns:
        list[str]: Element acronyms in data order.
    )pbdoc");

    m.def("from_string", &from_string, R"pbdoc(
      Create a Particle or Ion from a string label.

      Accepted forms:
        - Element acronym ("He", "C")
        - Isotope notation ("3He", "14C", "238U")
        - Special names "proton", "alpha", "neutron", "electron"

      Example:
        >>> ion = particles.from_string("14C")
        >>> ion.Z, ion.A
        (6, 14)

      Args:
        name (str): Element, isotope, or special particle label.

      Returns:
        Particle | particles.ions.Ion: The constructed object.

      Raises:
        ValueError: If the string cannot be parsed or is invalid.
      )pbdoc");
    m.def("from_pdg", &from_pdg, R"pbdoc(
      Create a Particle or Ion from a PDG code (nuclear encoding).

      Handles proton (2212), neutron (2112), and electron (11) explicitly.

      Args:
        pdg_code (int): PDG code.

      Returns:
        Particle | particles.ions.Ion: The constructed object.

      Raises:
        ValueError: If the PDG code is invalid.
      )pbdoc");




  auto ions_module = m.def_submodule("ions");
  init_ions(ions_module);
  m.attr("proton") = from_ZA(1,1);
  


  auto acronyms = get_acronyms();
  for (const auto& acronym : acronyms) {
    try {
      m.attr(acronym.c_str()) = from_string(acronym);
    } catch (const std::exception& e) {
      std::cerr << "Warning: Could not create particle " << acronym << ": " << e.what() << std::endl;
    }
  }

  m.def("from_ZA", &from_ZA, nb::arg("Z"), nb::arg("A"), R"pbdoc(
    Create an Ion from atomic number Z and mass number A.

    Args:
        Z (int): Atomic number.
        A (int): Mass number.

    Returns:
        particles.ions.Ion: The constructed ion.

    Raises:
        ValueError: If Z/A is not a supported isotope.
  )pbdoc");
}
