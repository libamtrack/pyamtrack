#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <map>
#include <stdexcept>

#include "../materials/materials.h"

namespace nb = nanobind;

enum class ElectronRangeModel : int {
  ButtsKatz = 2,
  Waligorski = 3,
  Geiss = 4,
  Scholz = 5,
  Edmund = 6,
  Tabata = 7,
  ScholzNew = 8,
};

/**
 * @brief Available electron range calculation models and their corresponding IDs.
 *
 * This map serves as the single source of truth for all supported models.
 * The string key is the model name used in Python, and the enum value carries
 * the corresponding model ID used in the underlying C/C++ implementation.
 */
inline const std::map<std::string, ElectronRangeModel> ELECTRON_RANGE_MODELS = {
    {"butts_katz", ElectronRangeModel::ButtsKatz},
    {"waligorski", ElectronRangeModel::Waligorski},
    {"geiss", ElectronRangeModel::Geiss},
    {"scholz", ElectronRangeModel::Scholz},
    {"edmund", ElectronRangeModel::Edmund},
    {"tabata", ElectronRangeModel::Tabata},
    {"scholz_new", ElectronRangeModel::ScholzNew},
};

/**
 * @brief Get a list of all available electron range calculation models.
 *
 * @return std::vector<std::string> Vector containing the names of all available models.
 */
std::vector<std::string> get_models();

/**
 * @brief Convert a model name to its corresponding typed model.
 *
 * @param model_name The name of the model as a string.
 * @return ElectronRangeModel The selected model.
 * @throws std::invalid_argument If the model name is not found.
 */
ElectronRangeModel parse_model_name(const std::string& model_name);

/**
 * @brief Convert a model name to its corresponding numerical ID.
 *
 * This preserves the existing Python model(name) API.
 */
int get_model_id(const std::string& model_name);

/**
 * @brief Validate and normalize a scalar, list, or integer NumPy-array model argument.
 */
void validate_model_argument(const nb::object& argument);
nb::object parse_model_argument(const nb::object& argument);

/**
 * @brief Calculate the maximum electron range in a material.
 *
 * This function calculates the maximum distance that electrons can travel
 * in a material before losing all their energy, using various theoretical
 * or empirical models.
 *
 * @param energy_MeV The electron kinetic energy in MeV. Can be a single value, NumPy array, or Python list.
 * @param material Either a material ID (int) or a Material object. Boolean values are not accepted.
 *                 Defaults to 1 (Liquid water).
 * @param model The electron range model to use. Can be specified as a string name, model ID,
 *              or ElectronRangeModel enum. Defaults to "tabata" (ID=7).
 * @param cartesian_product Parameter that tells whether to compute the cartesian product (all possible combinations) of
 * the preceding parameters
 * @return nb::object The calculated electron range(s) in meters. Returns a float when all inputs are
 *                   scalar, or a NumPy array when any input is a list or array.
 * @throws nb::type_error If material or model has an unsupported type, or if either is a bool.
 * @throws std::invalid_argument If the energy is negative/non-finite, or the model/material ID is invalid.
 */
nb::object electron_range(const nb::object& energy_MeV, const nb::object& material = nb::int_(1),
                          const nb::object& model = nb::str("tabata"), bool cartesian_product = false);

#endif  // ELECTRON_RANGE_H
