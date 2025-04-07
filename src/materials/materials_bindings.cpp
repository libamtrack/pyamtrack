#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "materials.h"
#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(materials, m) {
    m.doc() = "Functions and data structures for accessing and manipulating material properties.";

    py::class_<Material>(m, "Material")
        .def(py::init<long>())
        .def_readonly("id", &Material::id)
        .def_readonly("density_g_cm3", &Material::density_g_cm3)
        .def_readonly("I_eV", &Material::I_eV)
        .def_readonly("alpha_g_cm2_MeV", &Material::alpha_g_cm2_MeV)
        .def_readonly("p_MeV", &Material::p_MeV)
        .def_readonly("m_g_cm2", &Material::m_g_cm2)
        .def_readonly("average_A", &Material::average_A)
        .def_readonly("average_Z", &Material::average_Z)
        .def_readonly("name", &Material::name)
        .def_readonly("phase", &Material::phase);

    // Expose get_ids as a proper Python wrapper
    m.def("get_ids", []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        return std::vector<long>(std::begin(data.material_no), std::begin(data.material_no) + data.n);
    });

    m.def("get_names", []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        return std::vector<std::string>(std::begin(data.material_name), std::begin(data.material_name) + data.n);
    });

}