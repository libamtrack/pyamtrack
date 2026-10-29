#ifndef STOPPING_POWER_SOURCE_H
#define STOPPING_POWER_SOURCE_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <algorithm>
#include <cctype>
#include <optional>
#include <stdexcept>
#include <string>

extern "C" {
#include "AT_DataMaterial.h"
#include "AT_DataParticle.h"
#include "AT_StoppingPower.h"
#include "AT_StoppingPowerDataICRU.h"
#include "AT_StoppingPowerDataPSTAR.h"

extern AT_stopping_power_ICRU_table_struct AT_stopping_power_ICRU_table[2];
extern PSTAR_data_struct PSTAR_data;
}

namespace nb = nanobind;

/**
 * Stopping-power data source, matching libamtrack ids for Bethe / PSTAR / ICRU.
 *
 * Default is resolved per value: PSTAR when tabulated data cover the material
 * and energy (IDs 1-9), otherwise the analytical Bethe formula.
 */
enum class StoppingPowerSource { Default = 0, Bethe = 1, PSTAR = 2, ICRU = 3 };

struct StoppingPowerEnergyBounds {
  double minimum;
  double maximum;
};

inline const char* stopping_power_source_name(long source) {
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

/**
 * @brief Convert a source argument (StoppingPowerSource, int ID, or case-insensitive name) to its ID.
 */
inline long parse_stopping_power_source(const nb::object& source) {
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

inline long select_stopping_power_source(long source, long material_no, long particle_no) {
  if (source != static_cast<long>(StoppingPowerSource::Default)) {
    return source;
  }

  if (material_no >= Water_Liquid && material_no <= Copper) {
    return static_cast<long>(StoppingPowerSource::PSTAR);
  }

  return static_cast<long>(StoppingPowerSource::Bethe);
}

inline std::optional<StoppingPowerEnergyBounds> get_stopping_power_energy_bounds(long source, long material_no,
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

inline void validate_stopping_power_energy(double energy, long source, long material_no, long particle_no) {
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

/**
 * @brief Resolve a requested source to the concrete libamtrack source used for one energy/material/particle.
 *
 * "default" selects PSTAR where tabulated data exist and fall back to Bethe outside the PSTAR energy range.
 */
inline long resolve_stopping_power_source(long requested_source, double energy, long material_no, long particle_no) {
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

#endif  // STOPPING_POWER_SOURCE_H
