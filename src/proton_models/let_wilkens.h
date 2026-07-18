#ifndef LET_WILKENS_H
#define LET_WILKENS_H


#include <map>
#include <string>

#include "../materials/materials.h"
#include "../wrapper/multi_argument.h"
#include "../particles/particles.h"
#include "../particles/ions/ion.h"

enum class Averaging {
    Dose,
    Track
};

nb::object let_wilkens(nb::object depth_cm, nb::object material, double energy_MeV,
                       double energy_spread_fraction, nb::object averaging = nb::cast(std::string("dose"))
                       );

#endif  // LET_WILKENS_H