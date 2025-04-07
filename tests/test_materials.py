import pyamtrack

def test_material_initialization():
    material = pyamtrack.materials.Material(1)  # Water_Liquid
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 > 0

def test_get_ids():
    ids = pyamtrack.materials.get_ids()
    assert isinstance(ids, list)
    assert len(ids) > 0
    assert all(isinstance(id, int) for id in ids)
    assert ids[0] == 1  # Check the first ID

def test_get_long_names():
    names = pyamtrack.materials.get_long_names()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "Water, Liquid"  # Check the first name

def test_get_short_names():
    names = pyamtrack.materials.get_names()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "water_liquid"  # Check the first name

def test_via_object():
    material = pyamtrack.materials.water_liquid
    assert material.id == 1
    assert material.name == "Water, Liquid"
    assert material.density_g_cm3 == 1.0