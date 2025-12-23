import numpy as np
import pytest

import pyamtrack.stopping


def test_electron_range(electron_energy_MeV_high):
    """Test the electron_range function for various inputs."""
    range_m = pyamtrack.stopping.electron_range(electron_energy_MeV_high)
    assert range_m > 0.01, "Expected positive range for positive energy."


def test_electron_range_air_vs_water(electron_energy_MeV_high):
    """Test the electron_range function for air and water."""
    range_air = pyamtrack.stopping.electron_range(electron_energy_MeV_high, pyamtrack.materials.air)
    range_water = pyamtrack.stopping.electron_range(electron_energy_MeV_high, pyamtrack.materials.water_liquid)
    assert range_air > range_water, "Expected range in air to be larger than in water."


def test_material_assignment(electron_energy_MeV_high):
    """Test the material assignment by ID."""
    range_default = pyamtrack.stopping.electron_range(electron_energy_MeV_high)
    range_material_name = pyamtrack.stopping.electron_range(electron_energy_MeV_high, pyamtrack.materials.water_liquid)
    assert range_default == range_material_name, "Expected range to be the same for default and material name."
    range_material_id = pyamtrack.stopping.electron_range(electron_energy_MeV_high, 1)
    assert range_default == range_material_id, "Expected range to be the same for default and material ID."


def test_mixed_parameter_types(electron_energy_MeV_high):
    """Test passing each parameter as list or numpy.ndarray, check for output type and shape"""
    range_material_in_array = pyamtrack.stopping.electron_range(energy_MeV=[electron_energy_MeV_high])
    range_many_materials = pyamtrack.stopping.electron_range(electron_energy_MeV_high, [1, 2], 3)
    range_many_methods = pyamtrack.stopping.electron_range(
        electron_energy_MeV_high, pyamtrack.materials.water_liquid, [1, 2]
    )
    range_many_materials_and_methods = pyamtrack.stopping.electron_range(
        electron_energy_MeV_high, [0, 1, pyamtrack.materials.water_liquid], [3, 4, "tabata"]
    )
    assert isinstance(range_material_in_array, np.ndarray) and range_material_in_array.shape == (1,)
    assert isinstance(range_many_materials, np.ndarray) and range_many_materials.shape == (2,)
    assert isinstance(range_many_methods, np.ndarray) and range_many_methods.shape == (2,)
    assert isinstance(range_many_materials_and_methods, np.ndarray) and range_many_materials_and_methods.shape == (3,)


@pytest.mark.parametrize(
    "dtype1, dtype2",
    [
        (
            np.int64,
            np.int32,
        ),
        (
            np.int16,
            np.uint64,
        ),
        (
            np.int8,
            np.int16,
        ),
        (
            np.uint64,
            int,
        ),
        (np.uint32, np.int_),
        (
            np.short,
            np.int16,
        ),
    ],
)
def test_arrays_with_mixed_dtypes(dtype1: type, dtype2: type):
    """test passing numpy ndarray arguments of different integral dtypes."""
    range_numpy_arrays = pyamtrack.stopping.electron_range(
        [1000, 2000, 3000],
        np.array([0, 1, 2], dtype=dtype1),
        np.array([3, 4, 3], dtype=dtype2),
    )
    assert isinstance(range_numpy_arrays, np.ndarray) and range_numpy_arrays.shape == (3,)


def test_material_assignment_invalid(electron_energy_MeV_high):
    """Test the material assignment with an invalid ID."""
    with pytest.raises(
        RuntimeError,
        match="Material argument must be an integer or a pyamtrack.materials.Material object",
    ):
        pyamtrack.stopping.electron_range(electron_energy_MeV_high, "aaa")  # Invalid ID
    with pytest.raises(
        RuntimeError,
        match="Material argument must be an integer or a pyamtrack.materials.Material object",
    ):
        pyamtrack.stopping.electron_range(electron_energy_MeV_high, pyamtrack.materials.get_ids)


@pytest.mark.skip
def test_invalid_id(electron_energy_MeV_high):
    """Test the electron_range function with an invalid ID."""
    with pytest.raises(ValueError, match="Invalid material ID"):
        pyamtrack.stopping.electron_range(electron_energy_MeV_high, 1000000)
