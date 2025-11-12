import pytest

import pyamtrack
from tests.conftest import assert_list_of_type


def test_material_initialization_by_id():
    material = pyamtrack.materials.Material(1)  # Water_Liquid
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 > 0


def test_material_initialization_by_name():
    material = pyamtrack.materials.Material("Water, Liquid")
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 > 0


def test_material_initialization_by_invalid_id():
    with pytest.raises(ValueError):
        pyamtrack.materials.Material(9999)  # Invalid ID


def test_material_initialization_by_invalid_name():
    with pytest.raises(ValueError):
        pyamtrack.materials.Material("Invalid Material")  # Invalid name


def test_get_ids():
    ids = pyamtrack.materials.get_ids()
    assert_list_of_type(ids, int, expected_first_value=1)


def test_get_long_names():
    names = pyamtrack.materials.get_long_names()
    assert_list_of_type(names, str, expected_first_value="Water, Liquid")


def test_get_short_names():
    names = pyamtrack.materials.get_names()
    assert_list_of_type(names, str, expected_first_value="water_liquid")


def test_via_object():
    material = pyamtrack.materials.water_liquid
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 == 1.0
