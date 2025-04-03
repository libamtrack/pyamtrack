import pytest
import pyamtrack
import numpy as np


def test_electron_range_positive_energy():
    """Test the electron_range function with positive energy."""
    energy = 100.0  # MeV/u
    range_m = pyamtrack.electron_range(energy)
    assert range_m > 0.0, "Electron range should be positive for positive energy"

def test_electron_range_zero_energy():
    """Test the electron_range function with zero energy."""
    energy = 0.0  # MeV/u
    range_m = pyamtrack.electron_range(energy)
    assert range_m == 0.0, "Electron range should be 0 for zero energy"

def test_electron_range_negative_energy():
    """Test the electron_range function with negative energy."""
    energy = -10.0  # MeV/u
    # check if range is not-a-number (NaN)
    range_m = pyamtrack.electron_range(energy)
    assert np.isnan(range_m), "Electron range should be NaN for negative energy"

def test_electron_range_numpy_array():
    """Test the electron_range function with a NumPy array."""
    energies = np.array([10.0, 50.0, 100.0, 500.0])  # MeV/u
    ranges = pyamtrack.electron_range(energies)
    assert isinstance(ranges, np.ndarray), "Output should be a NumPy array"
    assert ranges.shape == energies.shape, "Output shape should match input shape"
    assert all(r > 0.0 for r in ranges), "All range values should be positive"

def test_electron_range_empty_numpy_array():
    """Test the electron_range function with an empty NumPy array."""
    energies = np.array([])  # Empty array
    ranges = pyamtrack.electron_range(energies)
    assert isinstance(ranges, np.ndarray), "Output should be a NumPy array"
    assert ranges.size == 0, "Output should be an empty array"

def test_electron_range_python_list():
    """Test the electron_range function with a Python list."""
    energies = [10.0, 50.0, 100.0, 500.0]  # MeV/u
    ranges = pyamtrack.electron_range(energies)
    assert isinstance(ranges, list), "Output should be a Python list"
    assert len(ranges) == len(energies), "Output length should match input length"
    assert all(r > 0.0 for r in ranges), "All range values should be positive"

def test_electron_range_empty_python_list():
    """Test the electron_range function with an empty Python list."""
    energies = []  # Empty list
    ranges = pyamtrack.electron_range(energies)
    assert isinstance(ranges, list), "Output should be a Python list"
    assert len(ranges) == 0, "Output should be an empty list"

def test_electron_range_invalid_numpy_array():
    """Test the electron_range function with an invalid NumPy array."""
    energies = np.array(["invalid", "data"])  # Invalid types
    with pytest.raises(ValueError, match="Input must be a float, int, NumPy array, or a Python list of numbers"):
        pyamtrack.electron_range(energies)

def test_electron_range_invalid_python_list():
    """Test the electron_range function with an invalid Python list."""
    energies = ["invalid", 100.0]  # Mixed types
    with pytest.raises(RuntimeError, match=r"Unable to cast Python instance of type <class 'str'> to C\+\+ type.*"):
        pyamtrack.electron_range(energies)