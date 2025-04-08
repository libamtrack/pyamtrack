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
        Calculate electron range in meters using various models.

        This function calculates the maximum electron range in a material using different theoretical 
        or empirical models. The range represents the maximum distance that electrons can travel 
        in the material before losing all their energy.

        Parameters
        ----------
        input : float or array_like
            The electron energy in MeV. Can be a single value, a NumPy array, or a Python list.
        material : int or Material, optional
            Either a material ID as integer or a Material object. Defaults to 1 (Liquid water).
        model : str or int, optional
            The stopping power model to use. Can be specified either as a string name or model ID.
            Available models:
            - "butts_katz" (id=2): Butts & Katz model
            - "waligorski" (id=3): Waligorski model
            - "geiss" (id=4): Geiss model
            - "scholz" (id=5): Scholz model
            - "edmund" (id=6): Edmund model
            - "tabata" (id=7): Tabata model (default)
            - "scholz_new" (id=8): Updated Scholz model

        Returns
        -------
        float or numpy.ndarray
            The calculated electron range(s) in meters. Returns a float for a single input,
            a NumPy array for a NumPy array input, or a Python list for a list input.

        Raises
        ------
        TypeError
            If material argument is neither an integer nor a Material object,
            or if model argument is neither a string nor an integer.
        ValueError
            If the input energy is negative or the model/material ID is invalid.
        )pbdoc"
    );
}