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

std::vector<long> get_ids();
std::vector<std::string> get_names();
std::vector<std::string> get_long_names();

class Material {
    public:
        long id;
        double density_g_cm3;
        double I_eV;
        double alpha_g_cm2_MeV;
        double p_MeV;
        double m_g_cm2;
        double average_A;
        double average_Z;
        std::string name;
        long phase;
    
        Material(long id);
        Material(const std::string &name);
    };
    

#endif // MATERIALS_H