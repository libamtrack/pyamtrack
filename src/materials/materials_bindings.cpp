#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <iostream>

#include "AT_DataMaterial.h"
#include "materials.h"

namespace nb = nanobind;

NB_MODULE(materials, m) {
  m.doc() = "Functions and data structures for accessing and manipulating material properties.";

  nb::class_<Material>(m, "Material", R"pbdoc(
        Represents a material with various physical properties.

        Attributes:
            id (int): The unique identifier for the material.
            density_g_cm3 (float): The density of the material in g/cm³.
            I_eV (float): The mean ionization potential in eV.
            alpha_g_cm2_MeV (float): Fit parameter for power-law representation of stopping power.
            p_MeV (float): Fit parameter for power-law representation of stopping power.
            m_g_cm2 (float): Fit parameter for linear representation of fluence changes.
            average_A (float): The average mass number of the material.
            average_Z (float): The average atomic number of the material.
            name (str): The name of the material.
            phase (int): The phase of the material (e.g., condensed or gaseous).
    )pbdoc")
      .def(nb::init<long>(), R"pbdoc(
            Initializes a Material object by ID.

            Args:
                id (int): The unique identifier for the material.
        )pbdoc")
      .def(nb::init<const std::string&>(), R"pbdoc(
            Initializes a Material object by name.

            Args:
                name (str): The name of the material.
        )pbdoc")
      .def_ro("id", &Material::id, "The unique identifier for the material.")
      .def_ro("density_g_cm3", &Material::density_g_cm3, "The density of the material in g/cm³.")
      .def_ro("I_eV", &Material::I_eV, "The mean ionization potential in eV.")
      .def_ro("alpha_g_cm2_MeV", &Material::alpha_g_cm2_MeV,
              "Fit parameter for power-law representation of stopping power.")
      .def_ro("p_MeV", &Material::p_MeV, "Fit parameter for power-law representation of stopping power.")
      .def_ro("m_g_cm2", &Material::m_g_cm2, "Fit parameter for linear representation of fluence changes.")
      .def_ro("average_A", &Material::average_A, "The average mass number of the material.")
      .def_ro("average_Z", &Material::average_Z, "The average atomic number of the material.")
      .def_ro("name", &Material::name, "The name of the material.")
      .def_ro("phase", &Material::phase, "The phase of the material (e.g., condensed or gaseous).");

  m.def(
      "get_ids",
      []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        return std::vector<long>(std::begin(data.material_no) + 1, std::begin(data.material_no) + data.n);
      },
      R"pbdoc(
        Retrieves the list of material IDs.

        Returns:
            list[int]: A list of material IDs.
    )pbdoc");

  m.def("get_long_names", &get_long_names, R"pbdoc(
        Retrieves the full names of all materials.

        Returns:
            list[str]: A list of full material names.
    )pbdoc");

  m.def("get_names", &get_names, R"pbdoc(
        Retrieves the sanitized names of all materials.

        The sanitized names replace spaces with underscores and remove non-alphanumeric characters.

        Returns:
            list[str]: A list of sanitized material names.
    )pbdoc");

  // Dynamically expose materials as attributes of the module
  auto names = get_names();
  for (size_t i = 0; i < names.size(); ++i) {
    m.attr(names[i].c_str()) = Material(static_cast<long>(i + 1));
  }
}
