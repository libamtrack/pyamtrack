#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "ion.h"
#include "ions.h"

namespace nb = nanobind;

void init_ions(nb::module_ &ions) {
  ions.doc() = "Ion-specific functionality.";

  nb::class_<Ion, Particle>(ions, "Ion", R"pbdoc(
        Represents an ion (charged particle).

        Inherits all properties and methods from Particle.
        Additionally exposes Z and A attributes.
        
        Use particles.from_string() to create Ion instances.
    )pbdoc")
    //   .def_static("from_AZ", &Ion::from_ZA, R"pbdoc(
    //     Create an Ion from atomic number Z and mass number A.
    // )pbdoc")
      .def_static("from_pdg", &Ion::from_pdg, R"pbdoc(
        Create an Ion from a PDG code.
    )pbdoc")
      .def_ro("Z", &Ion::Z, "The atomic number of the ion.")
      .def_ro("A", &Ion::A, "The mass number of the ion.")
      .def("__str__", [](const Ion &ion) {
        return "[Ion: " + std::to_string(ion.A) + ion.element_acronym + "]";
      })
      .def("__repr__", [](const Ion &ion) {
        return "[Ion " + ion.element_name + " (Z=" + std::to_string(ion.Z) + ", A=" + std::to_string(ion.A) + ")]";
      });
}
