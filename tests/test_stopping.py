import pytest
import pyamtrack.stopping


def test_electron_range():
    """Test the electron_range function for various inputs."""
    energy = 1000.0  # MeV
    range_m = pyamtrack.stopping.electron_range(energy)
    assert range_m > 0.01, "Expected positive range for positive energy."

def test_electron_range_air_vs_water():
    """Test the electron_range function for air and water."""
    energy = 1000.0  # MeV
    range_air = pyamtrack.stopping.electron_range(energy, pyamtrack.materials.air)
    range_water = pyamtrack.stopping.electron_range(energy, pyamtrack.materials.water_liquid)
    assert range_air > range_water, "Expected range in air to be larger than in water."

def test_material_assignment():
    """Test the material assignment by ID."""
    range_default = pyamtrack.stopping.electron_range(1000.0)
    range_material_name = pyamtrack.stopping.electron_range(1000.0, pyamtrack.materials.water_liquid)
    assert range_default == range_material_name, "Expected range to be the same for default and material name."
    range_material_id = pyamtrack.stopping.electron_range(1000.0, 1)
    assert range_default == range_material_id, "Expected range to be the same for default and material ID."

def test_material_assignment_invalid():
    """Test the material assignment with an invalid ID."""
    with pytest.raises(TypeError, match="Material argument must be either an integer or a Material object"):
        pyamtrack.stopping.electron_range(1000.0, "aaa")  # Invalid ID
    with pytest.raises(TypeError, match="Material argument must be either an integer or a Material object"):
        pyamtrack.stopping.electron_range(1000.0, pyamtrack.materials.get_ids)

@pytest.mark.skip
def test_invalid_id():
    """Test the electron_range function with an invalid ID."""
    with pytest.raises(ValueError, match="Invalid material ID"):
        pyamtrack.stopping.electron_range(1000.0, 1000000)