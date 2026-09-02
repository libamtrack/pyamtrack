#include "stopping_power.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_StoppingPower.h"
}

namespace {

constexpr long kPstarMaterialMin = 1;
constexpr long kPstarMaterialMax = 9;
constexpr long kIcruMaterialWater = 1;
constexpr long kIcruMaterialAluminumOxide = 2;

StoppingPowerSource parse_stopping_power_source(const nb::object& source) {
  if (nb::isinstance<StoppingPowerSource>(source)) {
    return nb::cast<StoppingPowerSource>(source);
  }
  if (nb::isinstance<nb::str>(source)) {
    std::string source_str = nb::cast<std::string>(source);
    std::transform(source_str.begin(), source_str.end(), source_str.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (source_str == "default") return StoppingPowerSource::Default;
    if (source_str == "bethe") return StoppingPowerSource::Bethe;
    if (source_str == "pstar") return StoppingPowerSource::PSTAR;
    if (source_str == "icru") return StoppingPowerSource::ICRU;
    throw std::invalid_argument("source must be \"default\", \"bethe\", \"pstar\", or \"icru\", got: " + source_str);
  }
  throw nb::type_error("source must be a StoppingPowerSource enum value or a string");
}

bool has_pstar_data(long material_id) {
  return material_id >= kPstarMaterialMin && material_id <= kPstarMaterialMax;
}

bool has_icru_data(long material_id) {
  return material_id == kIcruMaterialWater || material_id == kIcruMaterialAluminumOxide;
}

long resolve_source_no(StoppingPowerSource source, long material_id) {
  StoppingPowerSource resolved = source;
  if (resolved == StoppingPowerSource::Default) {
    resolved = get_default_stopping_power_source(material_id);
  }

  if (resolved == StoppingPowerSource::PSTAR && !has_pstar_data(material_id)) {
    throw std::invalid_argument("PSTAR stopping-power data are not available for material ID " +
                                std::to_string(material_id));
  }
  if (resolved == StoppingPowerSource::ICRU && !has_icru_data(material_id)) {
    throw std::invalid_argument("ICRU stopping-power data are not available for material ID " +
                                std::to_string(material_id));
  }

  return static_cast<long>(resolved);
}

template <typename LibStoppingFunction>
nb::object evaluate_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle,
                                   const nb::object& material, const nb::object& source, bool cartesian_product,
                                   LibStoppingFunction lib_function) {
  const StoppingPowerSource source_enum = parse_stopping_power_source(source);
  validate_material_argument(material);
  validate_particle_argument(particle);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV_u);
  arguments_vector.push_back(parse_particle_argument(particle));
  arguments_vector.push_back(parse_material_argument(material));

  auto compute = [source_enum, lib_function](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 3) {
      throw std::invalid_argument("Input vector must have at least three elements.");
    }
    double energy = variant_cast<double>(vec[0]);
    long particle_no = static_cast<long>(variant_cast<int>(vec[1]));
    long material_no = static_cast<long>(variant_cast<int>(vec[2]));

    if (energy <= 0.0) {
      throw std::invalid_argument("energy_MeV_u must be > 0, got: " + std::to_string(energy));
    }

    const long source_no = resolve_source_no(source_enum, material_no);
    const long n = 1;
    double result = 0.0;
    lib_function(source_no, n, &energy, &particle_no, material_no, &result);
    return result;
  };

  if (cartesian_product) return wrap_cartesian_product_function(compute, arguments_vector);
  return wrap_multiargument_function(compute, arguments_vector);
}

}  // namespace

StoppingPowerSource get_default_stopping_power_source(long material_id) {
  if (has_pstar_data(material_id)) return StoppingPowerSource::PSTAR;
  return StoppingPowerSource::Bethe;
}

nb::object mass_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                               const nb::object& source, bool cartesian_product) {
  return evaluate_stopping_power(energy_MeV_u, particle, material, source, cartesian_product,
                                 AT_Mass_Stopping_Power_with_no);
}

nb::object stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                          const nb::object& source, bool cartesian_product) {
  return evaluate_stopping_power(energy_MeV_u, particle, material, source, cartesian_product,
                                 AT_Stopping_Power_with_no);
}
