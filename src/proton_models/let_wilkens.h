#ifndef LET_WILKENS_H
#define LET_WILKENS_H


#include <map>
#include <string>

#include "../materials/materials.h"
#include "../wrapper/multi_argument.h"
#include "../particles/particles.h"
#include "../particles/ions/ion.h"


nb::object let_wilkens(nb::object depth_cm, long material, double energy_MeV, double energy_spread_MeV,
                       const std::string& averaging = "dose");

#endif  // LET_WILKENS_H