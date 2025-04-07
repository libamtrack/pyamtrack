#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "materials.h"

namespace py = pybind11;

PYBIND11_MODULE(materials, m) {
    m.doc() = "Materials";

    py::class_<Material>(m, "Material")
        .def(py::init<long>())
        .def_readonly("material_no", &Material::material_no)
        .def_readonly("density_g_cm3", &Material::density_g_cm3)
        .def_readonly("I_eV", &Material::I_eV)
        .def_readonly("alpha_g_cm2_MeV", &Material::alpha_g_cm2_MeV)
        .def_readonly("p_MeV", &Material::p_MeV)
        .def_readonly("m_g_cm2", &Material::m_g_cm2)
        .def_readonly("average_A", &Material::average_A)
        .def_readonly("average_Z", &Material::average_Z)
        .def_readonly("material_name", &Material::material_name)
        .def_readonly("phase", &Material::phase);

    m.def("get_material_name", [](long material_no) {
        char name[MATERIAL_NAME_LENGTH];
        AT_material_name_from_number(material_no, name);
        return std::string(name);
    });

}