#include <pybind11/pybind11.h>
#include "electron_range.h"

namespace py = pybind11;

PYBIND11_MODULE(stopping, m) {
    m.doc() = "Functions for calculating stopping power of ions and protons and range of particles in materials.";

   // Create submodule for models
   py::module_ models = m.def_submodule("models", "Stopping power models");
    
   // Add model constants using the map
   for (const auto& [name, id] : STOPPING_MODELS) {
       models.attr(name.c_str()) = py::int_(id);
   }

   m.def("get_models", &get_models, "Returns list of available stopping power models");
   m.def("model", &get_model_id, py::arg("name"), "Returns model ID for given model name");

    m.def(
        "electron_range",
        &electron_range,
        py::arg("input"),
        py::arg("material") = 1,
        py::arg("model") = py::str("tabata"),
        py::return_value_policy::automatic,
        R"pbdoc(
            Calculate electron range (m).
    
            Parameters:
                input (float | int | numpy.ndarray | list): The energy in MeV. Can be a single value, a NumPy array, or a Python list.
                material (int | Material, optional): Either a material ID as integer or a Material object. Defaults to 1 (Liquid water).
                model (str, optional): The stopping power model to use. Defaults to "tabata". Available models are: 
                    - "butts_katz"
                    - "waligorski"
                    - "geiss"
                    - "scholz"
                    - "edmund"
                    - "tabata"
                    - "scholz_new"
    
            Returns:
                float | numpy.ndarray | list: The calculated electron range(s). Returns a float for a single input, a NumPy array for a NumPy array input, or a Python list for a list input.
            )pbdoc"
    );
}