#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <map>

namespace py = pybind11;

// Single source of truth for model definitions
const std::map<std::string, int> STOPPING_MODELS = {
    {"butts_katz", 2},
    {"waligorski", 3},
    {"geiss", 4},
    {"scholz", 5},
    {"edmund", 6},
    {"tabata", 7},
    {"scholz_new", 8}
};

std::vector<std::string> get_models();
int get_model_id(const std::string& model_name);

py::object PYBIND11_EXPORT electron_range(py::object input, 
    py::object material = py::int_(1),
    py::object model = py::str("tabata")
);

#endif // ELECTRON_RANGE_H