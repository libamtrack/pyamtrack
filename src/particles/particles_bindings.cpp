#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <iostream>

#include "AT_DataParticle.h"
#include "particles.h"

namespace nb = nanobind;

NB_MODULE(particles, m) {
  m.doc() = "Functions and data structures for accessing and manipulating particle properties.";

  nb::class_<Particle>(m, "Particle", R"pbdoc(
        Represents a particle with various physical properties.

        Attributes:
        n (int): The number of particle.
        Z (int): Atomic number of the particle.
        atomic_weight (float): Atomic weight of the particle.
        element_name (str): Name of the particle.
        element_acronym (str): Acronym of the particle.
        density_g_cm3 (float): Density of the particle in g/cm³.
        I_eV_per_Z (float): Mean ionization potential per atomic number in eV/Z.
    )pbdoc")
      .def(nb::init<long>(), R"pbdoc(
        Initializes a Particle object by its number.

        Args:
            n (int): The number of the particle.
    )pbdoc")
      .def(nb::init<const std::string&>(), R"pbdoc(
        Initializes a Particle object by its acronym.

        Args:
            element_acronym (str): The acronym of the particle.
    )pbdoc")
      .def_ro("n", &Particle::n, "The number of the particle.")
      .def_ro("Z", &Particle::Z, "The atomic number of the particle.")
      .def_ro("atomic_weight", &Particle::atomic_weight, "The atomic weight of the particle.")
      .def_ro("element_name", &Particle::element_name, "The name of the particle.")
      .def_ro("element_acronym", &Particle::element_acronym, "The acronym of the particle.")
      .def_ro("density_g_cm3", &Particle::density_g_cm3, "The density of the particle in g/cm³.")
      .def_ro("I_eV_per_Z", &Particle::I_eV_per_Z, "The mean ionization potential per atomic number in eV/Z.");

  m.def("get_names", &get_names, R"pbdoc(
      Retrieves the names of all particles.

      Returns:
          list[str]: A list of particles names.
      )pbdoc");

  m.def("get_acronyms", &get_acronyms, R"pbdoc(
          Retrieves the acronyms of all particles.

          Returns:
              list[str]: A list of particles acronyms.
      )pbdoc");

  // Dynamically expose particles as attributes of the module
  auto names = get_names();
  auto acronyms = get_acronyms();
  for (size_t i = 0; i < names.size(); ++i) {
    m.attr(names[i].c_str()) = Particle(static_cast<long>(i + 1));
    m.attr(acronyms[i].c_str()) = Particle(static_cast<long>(i + 1));
  }
}
