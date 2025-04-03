#include "electron_range.h"
extern "C" {
    #include "AT_ElectronRange.h"
}

double electron_range(double E_MeV_u) {
    return AT_max_electron_range_m(E_MeV_u, 1, 7);
}