#ifndef MATERIALS_H
#define MATERIALS_H

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
#include "AT_DataMaterial.h"
}

namespace nb = nanobind;

/**
 * @brief Retrieves the list of material IDs.
 *
 * Example:
 * >>> get_ids()
 * [1, 2, 3, ..., 24]
 *
 * @return std::vector<long> A list of material IDs.
 */
std::vector<long> get_ids();

/**
 * @brief Retrieves the sanitized names of all materials.
 *
 * The sanitized names replace spaces with underscores and remove non-alphanumeric characters.
 *
 * Example:
 * >>> get_names()
 * ['water_liquid', 'aluminum_oxide', 'aluminum', ..., 'lead']
 *
 * @return std::vector<std::string> A list of sanitized material names.
 */
std::vector<std::string> get_names();

/**
 * @brief transforms material into its corresponding id. If int is passed as input, then returns input;
 *
 * @return int a material id
 */
inline int process_material(const nb::object& material) {
  int material_id = 0;
  if (PyBool_Check(material.ptr())) {
    throw nb::type_error("Material argument must be an integer (not bool) or a pyamtrack.materials.Material object");
  } else if (nb::isinstance<nb::int_>(material)) {
    material_id = nb::cast<int>(material);
  } else {
    try {
      nb::module_ pyamtrack_mod = nb::module_::import_("pyamtrack.materials");
      nb::object MaterialType = pyamtrack_mod.attr("Material");

      if (!nb::isinstance(material, MaterialType)) {
        throw nb::type_error("Material argument must be an integer or a pyamtrack.materials.Material object");
      }

      material_id = nb::cast<int>(material.attr("id"));

    } catch (const nb::python_error& e) {
      // FIX: Re-throw the original Python error
      throw;  // Preserves original Python exception type and traceback
    } catch (const nb::cast_error& e) {
      std::string error_msg = "Material object's 'id' attribute is not an integer: " + std::string(e.what());
      throw nb::type_error(error_msg.c_str());
    } catch (const std::exception& e) {  // Catches std::runtime_error and others
      std::string error_msg = "An error occurred while processing the Material object: " + std::string(e.what());
      throw std::runtime_error(error_msg.c_str());  // Nanobind will translate this
    }
  }
  return material_id;
}
/**
 * @brief Retrieves the full names of all materials.
 *
 * Example:
 * >>> get_long_names()
 * ['Water, Liquid', 'Aluminum Oxide', 'Aluminum', ..., 'Lead']
 *
 * @return std::vector<std::string> A list of full material names.
 */
std::vector<std::string> get_long_names();

/**
 * @class Material
 * @brief Represents a material with various physical properties.
 *
 * Example:
 * >>> material = Material(1)
 * >>> material.id
 * 1
 * >>> material.name
 * 'Water, Liquid'
 *
 * Attributes:
 * - id (int): The unique identifier for the material.
 * - density_g_cm3 (float): The density of the material in g/cm³.
 * - I_eV (float): The mean ionization potential in eV.
 * - alpha_g_cm2_MeV (float): Fit parameter for power-law representation of stopping power.
 * - p_MeV (float): Fit parameter for power-law representation of stopping power.
 * - m_g_cm2 (float): Fit parameter for linear representation of fluence changes.
 * - average_A (float): The average mass number of the material.
 * - average_Z (float): The average atomic number of the material.
 * - name (str): The name of the material.
 * - phase (int): The phase of the material (e.g., condensed or gaseous).
 */
class Material {
 public:
  long id;                /**< The unique identifier for the material. */
  double density_g_cm3;   /**< The density of the material in g/cm³. */
  double I_eV;            /**< The mean ionization potential in eV. */
  double alpha_g_cm2_MeV; /**< Fit parameter for power-law representation of stopping power. */
  double p_MeV;           /**< Fit parameter for power-law representation of stopping power. */
  double m_g_cm2;         /**< Fit parameter for linear representation of fluence changes. */
  double average_A;       /**< The average mass number of the material. */
  double average_Z;       /**< The average atomic number of the material. */
  std::string name;       /**< The name of the material. */
  long phase;             /**< The phase of the material (e.g., condensed or gaseous). */

  /**
   * @brief Initializes a Material object.
   *
   * Example:
   * >>> material = Material(1)
   * >>> material.id
   * 1
   * >>> material.name
   * 'Water, Liquid'
   *
   * @param id_or_name The unique identifier or name of the material.
   */
  Material(long id);

  /**
   * @brief Initializes a Material object using its name.
   *
   * Example:
   * >>> material = Material("Water, Liquid")
   * >>> material.id
   * 1
   * >>> material.name
   * 'Water, Liquid'
   *
   * @param name The name of the material.
   * @throws std::invalid_argument if the material name is not found.
   */
  Material(const std::string& name);
};

/**
 * @brief Validates a material argument, recursing into lists and NumPy arrays.
 *
 * This is the single point of material validation: callers must not re-check IDs
 * themselves, so that the accepted set of materials is defined by the libamtrack
 * material table alone. Material 0 ("User defined") is rejected because all of its
 * physical properties are zero.
 *
 * @throws std::invalid_argument (ValueError) if a material ID is not in the material table.
 * @throws nb::type_error if the argument is not an int, Material, list, or NumPy array.
 */
inline void validate_material_argument(const nb::object& argument) {
  if (nb::isinstance<nb::list>(argument)) {
    nb::list values = nb::cast<nb::list>(argument);
    for (size_t i = 0; i < values.size(); ++i) {
      validate_material_argument(values[i]);
    }
    return;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    validate_material_argument(argument.attr("tolist")());
    return;
  }

  long id;
  if (nb::isinstance<Material>(argument)) {
    id = nb::cast<Material>(argument).id;
  } else if (PyBool_Check(argument.ptr())) {
    throw nb::type_error("material must be an int (not bool), a Material, or a list / NumPy array of either");
  } else if (nb::isinstance<nb::int_>(argument)) {
    id = nb::cast<long>(argument);
  } else {
    throw nb::type_error("material must be an int, a Material, or a list / NumPy array of either");
  }

  if (id < 1 || AT_material_index_from_material_number(id) < 0) {
    throw std::invalid_argument("invalid material ID: " + std::to_string(id));
  }
}

inline nb::object parse_material_argument(const nb::object& argument) {
  if (nb::isinstance<Material>(argument)) {
    return nb::cast<long>(nb::cast<Material>(argument).id);
  }

  if (PyBool_Check(argument.ptr())) {
    throw nb::type_error("material must be an integer (not bool), Material, list, or NumPy array");
  }

  if (nb::isinstance<nb::int_>(argument)) {
    return argument;
  }

  if (nb::isinstance<nb::list>(argument)) {
    nb::list values = nb::cast<nb::list>(argument);
    nb::list parsed_values;
    for (size_t i = 0; i < values.size(); ++i) {
      parsed_values.append(parse_material_argument(values[i]));
    }
    return parsed_values;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    return parse_material_argument(argument.attr("tolist")());
  }

  throw nb::type_error("material must be an integer, Material, list, or NumPy array");
}

#endif  // MATERIALS_H
