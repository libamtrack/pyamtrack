#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <map>

#include "../materials/materials.h"

namespace nb = nanobind;

/**
 * @brief Available electron range calculation models and their corresponding IDs.
 *
 * This map serves as the single source of truth for all supported models.
 * The string key is the model name used in Python, and the integer value
 * is the corresponding model ID used in the underlying C/C++ implementation.
 */
const std::map<std::string, int> STOPPING_MODELS = {
    {"butts_katz", 2},  // Butts & Katz model
    {"waligorski", 3},  // Waligorski model
    {"geiss", 4},       // Geiss model
    {"scholz", 5},      // Scholz model
    {"edmund", 6},      // Edmund model
    {"tabata", 7},      // Tabata model (default)
    {"scholz_new", 8}   // Updated Scholz model
};

/**
 * @brief Get a list of all available electron range calculation models.
 *
 * @return std::vector<std::string> Vector containing the names of all available models.
 */
std::vector<std::string> get_models();

/**
 * @brief Convert a model name to its corresponding numerical ID.
 *
 * @param model_name The name of the model as a string.
 * @return int The numerical ID of the model.
 * @throws std::runtime_error If the model name is not found in STOPPING_MODELS.
 */
int get_model_id(const std::string& model_name);

/**
 * @brief Calculate the maximum electron range in a material.
 *
 * This function calculates the maximum distance that electrons can travel
 * in a material before losing all their energy, using various theoretical
 * or empirical models.
 *
 * @param input The electron energy in MeV. Can be a single value, NumPy array, or Python list.
 * @param material Either a material ID (int) or a Material object. Defaults to 1 (Liquid water).
 * @param model The stopping power model to use. Can be specified as a string name or model ID.
 *             Defaults to "tabata" (ID=7).
 * @return nb::object The calculated electron range(s) in meters. Returns a float for single input,
 *                   NumPy array for array input, or Python list for list input.
 * @throws nb::type_error If material argument is neither an integer nor a Material object,
 *                      or if model argument is neither a string nor an integer.
 * @throws std::runtime_error If the model name/ID is invalid.
 */
nb::object electron_range(const nb::object& input, const nb::object& material = nb::int_(1),
                          const nb::object& model = nb::str("tabata"));

#endif  // ELECTRON_RANGE_H
