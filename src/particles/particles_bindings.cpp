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
      .def(nb::init<>(), R"pbdoc(
        Initializes a Particle object by its internal ID.

        Args:
            id (int): The internal ID of the particle (1-based index).
    )pbdoc")
    //   .def_static("from_number", &Particle::from_number, R"pbdoc(
    //     Initializes a Particle object from a particle number (1000*Z + A).

    //     A particle number encodes the atomic number (Z) and mass number (A)
    //     according to the PyAmtrack convention:

    //         particle_no = 1000 * Z + A

    //     Example:
    //         >>> particle = Particle.from_number(6012)
    //         >>> particle.Z
    //         6
    //         >>> particle.A
    //         12

    //     Args:
    //         particle_no (int): The particle number in the format 1000*Z + A.

    //     Returns:
    //         Particle: A Particle object corresponding to the given particle number.

    //     Raises:
    //         ValueError: If the particle number is invalid.
    // )pbdoc")
      
      .def_static("from_AZ", &Particle::from_ZA, R"pbdoc(
        Create a Particle from atomic number Z and mass number A.
      )pbdoc")
      .def_static("from_pdg", &Particle::from_pdg, R"pbdoc(
        Create a Particle from a PDG code (nuclear encoding).
      )pbdoc")
      .def("info", [](const Particle &p) {
        std::cout << "[Particle Info]:\n"
                  << "  element = " << p.element_acronym << " (" << p.element_name << ")\n"
                  << "  id = " << p.id << "\n";
        if (p.pdg) {
          std::cout << "  PDG = " << *p.pdg << "\n";
        } else {
          std::cout << "  PDG = None\n";
        }
        std::cout << std::endl;
        return nb::none();
      })
      .def("__str__", &Particle::str, "String representation of the particle")
      .def("__repr__", &Particle::repr, "Detailed string representation of the particle")
      .def_ro("id", &Particle::id, "The internal ID of the particle.")
      .def_ro("atomic_weight", &Particle::atomic_weight, "The atomic weight of the particle.")
      .def_ro("element_name", &Particle::element_name, "The name of the particle.")
      .def_ro("element_acronym", &Particle::element_acronym, "The acronym of the particle.")
      // .def_ro("density_g_cm3", &Particle::density_g_cm3, "The density of the particle in g/cm³.")
      // .def_ro("I_eV_per_Z", &Particle::I_eV_per_Z, "The mean ionization potential per atomic number in eV/Z.")
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


  auto ions_module = m.def_submodule("ions");
  init_ions(ions_module);
  m.attr("proton") = &Particle::proton;
  
  auto acronyms = get_acronyms();
  for (const auto& acronym : acronyms) {
    try {
      m.attr(acronym.c_str()) = from_string(acronym);
    } catch (const std::exception& e) {
      std::cerr << "Warning: Could not create particle " << acronym << ": " << e.what() << "\n";
    }
  }


  // m.def("create", &create_particle, R"pbdoc(
  //     Factory function that creates either a Particle or Ion based on the isotope string.
      
  //     For special particles (proton, electron, neutron), returns a Particle.
  //     For regular atoms/ions, returns an Ion with Z and A set.
      
  //     Args:
  //         isotope (str): Isotope string (e.g., "12C", "He", "p", "e", "n").
      
  //     Returns:
  //         Particle or Ion: Appropriate particle type.
      
  //     Example:
  //         >>> ion = particles.create("12C")    # Returns Ion with Z=6, A=12
  //         >>> proton = particles.create("p")   # Returns Particle
  // )pbdoc");



  // Dynamically expose particles as attributes of the module
  // auto acronyms = get_acronyms();
  // for (size_t i = 0; i < acronyms.size(); ++i) {
  //   m.attr(acronyms[i].c_str()) = Particle(acronyms[i]);
  // }
}
