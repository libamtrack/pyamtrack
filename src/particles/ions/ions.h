#pragma once
#include <nanobind/nanobind.h>
#include "ion.h"

namespace nb = nanobind;

void init_ions(nb::module_ &ions);

Ion from_ZA(long long Z, long long A);