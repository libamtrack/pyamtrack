#ifndef BORTFELD_PROTON_RANGE_H
#define BORTFELD_PROTON_RANGE_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

/**
 * @brief Calculate proton range using Bortfeld dose model.
 *
 * Calculates the range where dose drops to a specified fraction of maximum value.
 *
 * @param energy_MeV Initial kinetic energy of proton beam in MeV/u.
 * @param sigma_E_MeV Kinetic energy spread (standard deviation) in MeV/u.
 *                     If negative, defaults to 0.01 * energy_MeV.
 * @param material Material ID (int). Defaults to 1 (Liquid water).
 * @param eps Fraction of primary fluence contributing to the tail of energy spectrum.
 *            If negative, defaults to 0.03.
 * @param dose_drop Fraction of max dose at which range is calculated.
 *                  If negative, defaults to 0.8.
 * @return The calculated range in cm.
 */
double bortfeld_proton_range(const double energy_MeV, const double sigma_E_MeV, const long material, const double eps,
                             const double dose_drop);

#endif  // BORTFELD_PROTON_RANGE_H
