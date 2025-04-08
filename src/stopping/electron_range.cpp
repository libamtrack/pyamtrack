#include "../wrapper_template.h"
#include "electron_range.h"
#include <iostream>

extern "C" {
    #include "AT_ElectronRange.h"
}

// Helper function for single value calculation
double electron_range_single(double E_MeV_u) {
    return AT_max_electron_range_m(E_MeV_u, 1, 7);
}

// Main function to handle different input types
py::object electron_range(py::object input, py::object material = py::int_(1)) {
    int material_id = 0;
    if (py::isinstance<py::int_>(material)) {
        material_id = material.cast<int>();
    } else {
        // Check if material is an instance of Material class
        py::module_ builtins = py::module_::import("builtins");
        py::object type_func = builtins.attr("type");
        py::str type_name = type_func(material).attr("__name__").cast<py::str>();
        
        if (std::string(type_name) != "Material") {
            throw py::type_error("Material argument must be either an integer or a Material object");
        }
        
        try {
            material_id = material.attr("id").cast<int>();
        } catch (const py::error_already_set&) {
            throw py::type_error("Material object must have 'id' attribute");
        }
    }
    return wrap_function(electron_range_single, input);
}