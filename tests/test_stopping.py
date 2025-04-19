import pytest
import pyamtrack.stopping


@pytest.fixture
def electron_energy_MeV():
    """Fixture providing the electron energy in MeV for tests."""
    return 1000.0


def test_electron_range(electron_energy_MeV):
    """Test the electron_range function for various inputs."""
    range_m = pyamtrack.stopping.electron_range(electron_energy_MeV)
    assert range_m > 0.01, "Expected positive range for positive energy."

 
def test_electron_range_air_vs_water(electron_energy_MeV):
    """Test the electron_range function for air and water."""
    range_air = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.air)
    range_water = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.water_liquid)
    assert range_air > range_water, "Expected range in air to be larger than in water."


def test_material_assignment(electron_energy_MeV):
    """Test the material assignment by ID."""
    range_default = pyamtrack.stopping.electron_range(electron_energy_MeV)
    range_material_name = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.water_liquid)
    assert range_default == range_material_name, "Expected range to be the same for default and material name."
    range_material_id = pyamtrack.stopping.electron_range(electron_energy_MeV, 1)
    assert range_default == range_material_id, "Expected range to be the same for default and material ID."


def test_material_assignment_invalid(electron_energy_MeV):
    """Test the material assignment with an invalid ID."""
    with pytest.raises(RuntimeError, match="Material argument must be an integer or a pyamtrack.materials.Material object"):
        pyamtrack.stopping.electron_range(electron_energy_MeV, "aaa")  # Invalid ID
    with pytest.raises(RuntimeError, match="Material argument must be an integer or a pyamtrack.materials.Material object"):
        pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.get_ids)


@pytest.mark.skip
def test_invalid_id(electron_energy_MeV):
    """Test the electron_range function with an invalid ID."""
    with pytest.raises(ValueError, match="Invalid material ID"):
        pyamtrack.stopping.electron_range(electron_energy_MeV, 1000000)