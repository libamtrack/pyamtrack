import pytest
from pyamtrack.materials import Material

def test_material_initialization():
    material = Material(1)  # Water_Liquid
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 > 0
