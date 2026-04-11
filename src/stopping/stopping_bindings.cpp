#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

namespace nb = nanobind;

NB_MODULE(stopping, m) {
  m.doc() = "Functions for calculating stopping power of ions and protons in materials.";

  // Placeholder: stopping power functions will be added here later
  // For now, the module is empty but must be defined for the build system to work
}
