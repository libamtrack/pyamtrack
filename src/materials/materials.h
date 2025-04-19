#ifndef MATERIALS_H
#define MATERIALS_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
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
        long id; /**< The unique identifier for the material. */
        double density_g_cm3; /**< The density of the material in g/cm³. */
        double I_eV; /**< The mean ionization potential in eV. */
        double alpha_g_cm2_MeV; /**< Fit parameter for power-law representation of stopping power. */
        double p_MeV; /**< Fit parameter for power-law representation of stopping power. */
        double m_g_cm2; /**< Fit parameter for linear representation of fluence changes. */
        double average_A; /**< The average mass number of the material. */
        double average_Z; /**< The average atomic number of the material. */
        std::string name; /**< The name of the material. */
        long phase; /**< The phase of the material (e.g., condensed or gaseous). */

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
        Material(const std::string &name);
};

#endif // MATERIALS_H