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

nb::object mass_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                               const nb::object& source, bool cartesian_product) {
  validate_particle_argument(particle);
  validate_material_argument(material);

  long particle_no = nb::cast<long>(parse_particle_argument(particle));
  long material_no = nb::cast<long>(parse_material_argument(material));
  long source_id = select_stopping_power_source(parse_stopping_power_source(source), material_no, particle_no);

  double energy_value = nb::cast<double>(energy_MeV_u);
  double result;
  AT_Mass_Stopping_Power_with_no(source_id, 1, &energy_value, &particle_no, material_no, &result);
  return nb::cast(result);
}

// nb::object stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
//                           const nb::object& source, bool cartesian_product) {
//   return evaluate_stopping_power(energy_MeV_u, particle, material, source, cartesian_product,
//                                  AT_Stopping_Power_with_no);
// }

long parse_stopping_power_source(const nb::object& source) {
  if (nb::isinstance<long>(source)) {
    long source_id = nb::cast<long>(source);
    if (source_id < 0 || source_id > 3) {
      throw std::invalid_argument("Invalid stopping power source ID: " + std::to_string(source_id));
    }
    return source_id;
  } else if (nb::isinstance<StoppingPowerSource>(source)) {
    return static_cast<long>(nb::cast<StoppingPowerSource>(source));
  } else if (nb::isinstance<std::string>(source)) {
    std::string source_str = nb::cast<std::string>(source);
    std::transform(source_str.begin(), source_str.end(), source_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (source_str == "default") {
      return static_cast<long>(StoppingPowerSource::Default);
    } else if (source_str == "bethe") {
      return static_cast<long>(StoppingPowerSource::Bethe);
    } else if (source_str == "pstar") {
      return static_cast<long>(StoppingPowerSource::PSTAR);
    } else if (source_str == "icru") {
      return static_cast<long>(StoppingPowerSource::ICRU);
    } else {
      throw std::invalid_argument("Invalid stopping power source string: " + source_str);
    }
  } else {
    throw std::invalid_argument("Invalid stopping power source type. Must be StoppingPowerSource enum or string.");
  }
}

long select_stopping_power_source(long source, long material_no, long particle_no) {
  if (source != static_cast<long>(StoppingPowerSource::Default)) {
    return source;
  }

  if (material_no >= Water_Liquid && material_no <= Copper) {
    return static_cast<long>(StoppingPowerSource::PSTAR);
  }

  return static_cast<long>(StoppingPowerSource::Bethe);
}
