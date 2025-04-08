#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <map>

namespace py = pybind11;

// Single source of truth for model definitions
const std::map<std::string, int> STOPPING_MODELS = {
    {"katz", 2},
    {"tabata", 7}
};

std::vector<std::string> get_models();
int get_model_id(const std::string& mode#include <pybind11/stl.h>l_name);

py::object PYBIND11_EXPORT electron_range(py::object input, py::object material = py::int_(1));

#endif // ELECTRON_RANGE_H