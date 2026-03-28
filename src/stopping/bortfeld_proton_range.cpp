#include "bortfeld_proton_range.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
extern "C" {
#include "AT_ProtonAnalyticalBeamParameters.h"
}
#pragma GCC diagnostic pop

double bortfeld_proton_range(const double energy_MeV, const double sigma_E_MeV, const long material, const double eps,
                             const double dose_drop) {
  // Call the libamtrack C function with search_direction hardcoded to 1 (distal side)
  return AT_range_Bortfeld_cm(energy_MeV, sigma_E_MeV, material, eps, dose_drop, 1);
}
