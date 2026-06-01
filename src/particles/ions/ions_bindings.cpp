#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "ion.h"
#include "ions.h"

namespace nb = nanobind;

void init_ions(nb::module_ &ions) {
  ions.doc() = "Ion-specific functionality.";

  nb::class_<Ion, Particle>(ions, "Ion", R"pbdoc(
        Represents an ion (nucleus) with atomic number Z and mass number A.

        Inherits all properties and methods from Particle, and adds:
            - Z: atomic number
            - A: mass number

        Prefer using particles.from_string() or particles.from_ZA() to
        create ions. Use the constructor for PDG-code-based creation.
    )pbdoc")
      .def(nb::init<long long>(), R"pbdoc(
        Create an Ion from a PDG code.

        Args:
            pdg_code (int): PDG code for a nucleus.

        Raises:
            ValueError: If the PDG code is invalid or corresponds to a non-ion.
      )pbdoc")
      .def_ro("Z", &Ion::Z, "Atomic number (Z).")
      .def_ro("A", &Ion::A, "Mass number (A).")
      .def("__str__", [](const Ion &ion) {
        return std::to_string(ion.A) + ion.element_acronym;  // e.g. "4He", "12C"
      })
      .def("__repr__", [](const Ion &ion) {
        return "Ion(element_name=\"" + ion.element_name +
          "\", element_acronym=\"" + ion.element_acronym +
          "\", Z=" + std::to_string(ion.Z) +
          ", A=" + std::to_string(ion.A) + ")";
      });
}
