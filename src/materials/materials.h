#ifndef MATERIALS_H
#define MATERIALS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>

extern "C" {
    #include "AT_DataMaterial.h"
}

namespace py = pybind11;


class Material {
    public:
        long material_no;
        double density_g_cm3;
        double I_eV;
        double alpha_g_cm2_MeV;
        double p_MeV;
        double m_g_cm2;
        double average_A;
        double average_Z;
        std::string material_name;
        long phase;
    
        Material(long material_no)
            : material_no(material_no) {
            AT_get_material_data(material_no, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
            char name[MATERIAL_NAME_LENGTH];
            AT_material_name_from_number(material_no, name);
            material_name = std::string(name);
            phase = AT_phase_from_material_no(material_no);
        }
    };
    

#endif // MATERIALS_H