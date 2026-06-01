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

        Use particles.from_string() to create particle instances.
        Pre-instantiated particles are available as module attributes (e.g., particles.proton, particles.C, particles.He).

        Attributes:
            id (int): Internal ID of the particle (row index in AT_Particle_Data).
            atomic_weight (float): Atomic weight of the particle.
            element_name (str): Name of the particle.
            element_acronym (str): Acronym of the particle.
    )pbdoc")
      .def(nb::init<long long>(), R"pbdoc()pbdoc")
      

      
      // .def("info", [](const Particle &p) {
      //   std::cout << "[Particle Info]:" << std::endl
      //             << "  element = " << p.element_acronym << " (" << p.element_name << ")" << std::endl
      //             << "  id = " << p.id << std::endl;
      //   if (p.pdg) {
      //     std::cout << "  PDG = " << *p.pdg << std::endl;
      //   } else {
      //     std::cout << "  PDG = None" << std::endl;
      //   }
      //   std::cout << std::endl;
      //   return nb::none();
      // })
      .def("__str__", &Particle::str, "String representation of the particle")
      .def("__repr__", &Particle::repr, "Detailed string representation of the particle")
      .def_ro("id", &Particle::id, "The internal ID of the particle.")
      .def_ro("atomic_weight", &Particle::atomic_weight, "The atomic weight of the particle.")
      .def_ro("element_name", &Particle::element_name, "The name of the particle.")
      .def_ro("element_acronym", &Particle::element_acronym, "The acronym of the particle.")
      .def_ro("pdg", &Particle::pdg, "The PDG code of the particle.")
      ;

  m.def("get_names", &get_names, R"pbdoc(
      Retrieves the names of all particles.

      Returns:
          list[str]: A list of particle names.
  )pbdoc");

  m.def("get_acronyms", &get_acronyms, R"pbdoc(
      Retrieves the acronyms of all particles.

      Returns:
          list[str]: A list of particle acronyms.
  )pbdoc");

  m.def("from_string", &from_string, R"pbdoc(
        Initializes a Particle object from a string representation.

        The string can be:
            - An acronym of the element ("He", "C")
            - An isotope notation with mass number ("3He", "14C", "238U")

        Example:
            >>> particle = Particle.from_string("14C")
            >>> particle.Z
            6
            >>> particle.A
            14
            >>> particle.element_name
            'Carbon'

        Args:
            name (str): The string representation of the particle.

        Returns:
            Particle: A Particle object corresponding to the given name.

        Raises:
            ValueError: If the string cannot be parsed.
      )pbdoc");
  m.def("from_pdg", &from_pdg, R"pbdoc(
        Create a Particle from a PDG code (nuclear encoding).
      )pbdoc");




  auto ions_module = m.def_submodule("ions");
  init_ions(ions_module);
  m.attr("proton") = from_ZA(1,1);
  


  auto acronyms = get_acronyms();
  for (const auto& acronym : acronyms) {
    try {
      m.attr(acronym.c_str()) = from_string(acronym);
    } catch (const std::exception& e) {
      std::cerr << "Warning: Could not create particle " << acronym << ": " << e.what() << "\n";
    }
  }

  m.def("from_ZA", &from_ZA, nb::arg("Z"), nb::arg("A"), R"pbdoc(
    Create an Ion from atomic number Z and mass number A.
  )pbdoc");
}
