#include "stopping_power.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_StoppingPower.h"
#include "AT_StoppingPowerDataICRU.h"
#include "AT_StoppingPowerDataPSTAR.h"

extern AT_stopping_power_ICRU_table_struct AT_stopping_power_ICRU_table[2];
extern PSTAR_data_struct PSTAR_data;
}

using StoppingPowerFunction = int (*)(const long, const long, const double[], const long[], const long, double[]);

namespace {

struct StoppingPowerEnergyBounds {
  double minimum;
  double maximum;
};

const char* stopping_power_source_name(long source) {
  switch (source) {
    case static_cast<long>(StoppingPowerSource::Bethe):
      return "Bethe";
    case static_cast<long>(StoppingPowerSource::PSTAR):
      return "PSTAR";
    case static_cast<long>(StoppingPowerSource::ICRU):
      return "ICRU";
    default:
      return "unknown";
  }
}

std::optional<StoppingPowerEnergyBounds> get_stopping_power_energy_bounds(long source, long material_no,
                                                                          long particle_no) {
  switch (source) {
    case static_cast<long>(StoppingPowerSource::Bethe):
      return std::nullopt;

    case static_cast<long>(StoppingPowerSource::PSTAR): {
      if (material_no < 0 || material_no >= N_PSTAR_MATERIALS ||
          PSTAR_data.stopping_power_source_data[material_no] == nullptr) {
        throw std::invalid_argument("PSTAR data is not available for material ID " + std::to_string(material_no));
      }
      const auto* table = PSTAR_data.stopping_power_source_data[material_no];
      return StoppingPowerEnergyBounds{table->energy_and_stopping_power[0][0],
                                       table->energy_and_stopping_power[N_PSTAR_DATAPOINTS - 1][0]};
    }

    case static_cast<long>(StoppingPowerSource::ICRU): {
      if (material_no != Water_Liquid && material_no != Aluminum_Oxide) {
        throw std::invalid_argument("ICRU data is not available for material ID " + std::to_string(material_no));
      }

      const auto& table = AT_stopping_power_ICRU_table[material_no - 1];
      const long atomic_number = AT_Z_from_particle_no_single(particle_no);
      if (atomic_number > 18) {
        throw std::invalid_argument("ICRU data is not available for particles with atomic number above 18");
      }

      double maximum = table.energy_and_stopping_power[0][table.number_of_data_points - 1];
      if (atomic_number == 2) {
        maximum = 250.0;
      }
      return StoppingPowerEnergyBounds{table.energy_and_stopping_power[0][0], maximum};
    }

    default:
      throw std::invalid_argument("Invalid resolved stopping power source ID: " + std::to_string(source));
  }
}

void validate_stopping_power_energy(double energy, long source, long material_no, long particle_no) {
  const auto bounds = get_stopping_power_energy_bounds(source, material_no, particle_no);
  if (!bounds.has_value()) {
    return;
  }

  if (energy < bounds->minimum || energy > bounds->maximum) {
    throw std::invalid_argument("energy_MeV_u=" + std::to_string(energy) + " is outside the " +
                                stopping_power_source_name(source) + " range [" + std::to_string(bounds->minimum) +
                                ", " + std::to_string(bounds->maximum) + "] MeV/u");
  }
}

long resolve_stopping_power_source(long requested_source, double energy, long material_no, long particle_no) {
  const long selected_source = select_stopping_power_source(requested_source, material_no, particle_no);
  if (requested_source == static_cast<long>(StoppingPowerSource::Default) &&
      selected_source == static_cast<long>(StoppingPowerSource::PSTAR)) {
    const auto bounds = get_stopping_power_energy_bounds(selected_source, material_no, particle_no);
    if (bounds.has_value() && (energy < bounds->minimum || energy > bounds->maximum)) {
      return static_cast<long>(StoppingPowerSource::Bethe);
    }
  }
  return selected_source;
}

nb::object make_source_metadata(const nb::object& values, const std::vector<long>& source_ids) {
  if (!nb::isinstance<nb::ndarray<>>(values)) {
    if (source_ids.size() != 1) {
      throw std::runtime_error("Internal error: scalar stopping-power result has unexpected metadata size.");
    }
    return nb::cast(source_ids.front());
  }

  auto values_array = nb::cast<nb::ndarray<>>(values);
  if (values_array.size() != source_ids.size()) {
    throw std::runtime_error("Internal error: stopping-power metadata size does not match the result.");
  }

  auto* metadata = new long[source_ids.size()];
  std::copy(source_ids.begin(), source_ids.end(), metadata);
  nb::capsule owner(metadata, [](void* pointer) noexcept { delete[] static_cast<long*>(pointer); });

  std::vector<size_t> shape;
  shape.reserve(values_array.ndim());
  for (size_t i = 0; i < values_array.ndim(); ++i) {
    shape.push_back(values_array.shape(i));
  }

  return nb::ndarray<long, nb::numpy>(metadata, shape.size(), shape.data(), owner).cast();
}

nb::object make_full_output_result(const nb::object& values, const nb::object& source_metadata) {
  nb::object named_tuple_factory = nb::module_::import_("collections").attr("namedtuple");
  nb::object result_type = named_tuple_factory("StoppingPowerResult", nb::make_tuple("value", "source_id"));
  return result_type(values, source_metadata);
}

double stopping_power_scalar(bool allow_bethe_fallback, bool full_output,
                             StoppingPowerFunction stopping_power_function, std::vector<long>& resolved_sources,
                             const std::vector<std::variant<double, int>>& values) {
  if (values.size() < 4) {
    throw std::invalid_argument("Stopping-power input must contain energy, particle, material, and source.");
  }

  double energy = variant_cast<double>(values[0]);
  long particle_no = variant_cast<long>(values[1]);
  long material_no = variant_cast<long>(values[2]);
  long source = variant_cast<long>(values[3]);

  if (!std::isfinite(energy) || energy <= 0.0) {
    throw std::invalid_argument("energy_MeV_u must be > 0 and finite");
  }

  source = resolve_stopping_power_source(source, energy, material_no, particle_no);
  if (source == static_cast<long>(StoppingPowerSource::Bethe) && !allow_bethe_fallback) {
    throw std::invalid_argument("Bethe fallback is not allowed, but the selected source has no tabular data for the material and energy");
  }
  validate_stopping_power_energy(energy, source, material_no, particle_no);

  double result = 0.0;
  const int status = stopping_power_function(source, 1, &energy, &particle_no, material_no, &result);
  if (status != AT_Success || result < 0.0) {
    if (allow_bethe_fallback && source != static_cast<long>(StoppingPowerSource::Bethe)) {
      source = static_cast<long>(StoppingPowerSource::Bethe);
      const int fallback_status = stopping_power_function(source, 1, &energy, &particle_no, material_no, &result);
      if (fallback_status != AT_Success || result < 0.0) {
        throw std::invalid_argument("Stopping-power calculation failed for the selected source and material");
      }
    } else {
      throw std::invalid_argument("Stopping-power calculation failed for the selected source and material");
    }
  }
  if (full_output) {
    resolved_sources.push_back(source);
  }
  return result;
}

// wrapper function to evaluate stopping power using the provided stopping_power_function
// shared code between mass_stopping_power and stopping_power to avoid duplication
nb::object evaluate_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle,
                                   const nb::object& material, const nb::object& source, bool cartesian_product,
                                   bool allow_bethe_fallback, bool full_output,
                                   StoppingPowerFunction stopping_power_function) {
  validate_particle_argument(particle);
  validate_material_argument(material);

  const long requested_source = parse_stopping_power_source(source);
  std::vector<long> resolved_sources;

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV_u);
  arguments_vector.push_back(parse_particle_argument(particle));
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(nb::cast(requested_source));

  MultiargumentFunc scalar_function =
      std::bind(stopping_power_scalar, allow_bethe_fallback, full_output, stopping_power_function,
                std::ref(resolved_sources), std::placeholders::_1);

  nb::object values;
  if (cartesian_product) {
    values = wrap_cartesian_product_function(scalar_function, arguments_vector);
  } else {
    values = wrap_multiargument_function(scalar_function, arguments_vector);
  }

  if (!full_output) {
    return values;
  }
  return make_full_output_result(values, make_source_metadata(values, resolved_sources));
}

}  // namespace

nb::object mass_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                               const nb::object& source, bool cartesian_product, bool allow_bethe_fallback,
                               bool full_output) {
  return evaluate_stopping_power(energy_MeV_u, particle, material, source, cartesian_product, allow_bethe_fallback,
                                 full_output, AT_Mass_Stopping_Power_with_no);
}

nb::object stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                          const nb::object& source, bool cartesian_product, bool allow_bethe_fallback,
                          bool full_output) {
  return evaluate_stopping_power(energy_MeV_u, particle, material, source, cartesian_product, allow_bethe_fallback,
                                 full_output, AT_Stopping_Power_with_no);
}

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
