#ifndef LET_WILKENS_H
#define LET_WILKENS_H


#include <map>

#include "../materials/materials.h"
#include "../wrapper/multi_argument.h"
#include "../particles/particles.h"
#include "../particles/ions/ion.h"



// nb::object let_track(nb::object depth_cm, long material, double energy_MeV, double energy_spread_MeV);


nb::object let_dose(nb::object depth_cm, long material, double energy_MeV, double energy_spread_MeV);

#endif  // LET_WILKENS_H