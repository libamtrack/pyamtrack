#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "AT_DataParticle.h"
#include "particles.h"

namespace nb = nanobind;

NB_MODULE(particles, m) {
  m.doc() = "Functions and data structures for accessing and manipulating particle properties.";

  nb::class_<Particle>(m, "Particle", R"pbdoc(
        Represents a particle with various physical properties.

        Attributes:
            id (int): Internal ID of the particle (row index in AT_Particle_Data).
            Z (int): Atomic number of the particle.
            A (int): Mass number of the particle.
            atomic_weight (float): Atomic weight of the particle.
            element_name (str): Name of the particle.
            element_acronym (str): Acronym of the particle.
            density_g_cm3 (float): Density of the particle in g/cm³.
            I_eV_per_Z (float): Mean ionization potential per atomic number in eV/Z.
    )pbdoc")
      .def(nb::init<long>(), R"pbdoc(
        Initializes a Particle object by its internal ID.

        Args:
            id (int): The internal ID of the particle (1-based index).
    )pbdoc")
      .def(nb::init<const std::string&>(), R"pbdoc(
        Initializes a Particle object by its acronym.

        Args:
            element_acronym (str): The acronym of the particle.
    )pbdoc")
      .def_static("from_number", &Particle::from_number, R"pbdoc(
        Initializes a Particle object from a particle number (1000*Z + A).

        A particle number encodes the atomic number (Z) and mass number (A)
        according to the PyAmtrack convention:

            particle_no = 1000 * Z + A

        Example:
            >>> particle = Particle.from_number(6012)
            >>> particle.Z
            6
            >>> particle.A
            12

        Args:
            particle_no (int): The particle number in the format 1000*Z + A.

        Returns:
            Particle: A Particle object corresponding to the given particle number.

        Raises:
            ValueError: If the particle number is invalid.
    )pbdoc")
      .def_static("from_string", &Particle::from_string, R"pbdoc(
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
      )pbdoc")
      .def_ro("id", &Particle::id, "The internal ID of the particle.")
      .def_ro("Z", &Particle::Z, "The atomic number of the particle.")
      .def_prop_ro("A", &Particle::py_get_A,
                   "The mass number of the particle. Available if constructed via from_number(), else None.")
      .def_ro("atomic_weight", &Particle::atomic_weight, "The atomic weight of the particle.")
      .def_ro("element_name", &Particle::element_name, "The name of the particle.")
      .def_ro("element_acronym", &Particle::element_acronym, "The acronym of the particle.")
      .def_ro("density_g_cm3", &Particle::density_g_cm3, "The density of the particle in g/cm³.")
      .def_ro("I_eV_per_Z", &Particle::I_eV_per_Z, "The mean ionization potential per atomic number in eV/Z.");

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

  // Dynamically expose particles as attributes of the module
  auto names = get_names();
  auto acronyms = get_acronyms();
  for (size_t i = 0; i < names.size(); ++i) {
    m.attr(names[i].c_str()) = Particle(static_cast<long>(i + 1));
    m.attr(acronyms[i].c_str()) = Particle(static_cast<long>(i + 1));
  }
}
