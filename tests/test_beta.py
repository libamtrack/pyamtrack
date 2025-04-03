import pytest
import numpy as np
from pyamtrack.converters import beta_from_energy


def test_beta_from_energy():
    """Test the beta_from_energy function."""
    energy = 100.0  # MeV/u
    beta = beta_from_energy(energy)
    assert 0.0 < beta < 1.0, "Beta should be between 0 and 1"

def test_beta_zero_energy():
    """Test the beta_from_energy function with zero energy."""
    energy = 0.0  # MeV/u
    beta = beta_from_energy(energy)
    assert beta == 0.0, "Beta should be 0 for zero energy"

def test_beta_negative_energy():
    """Test the beta_from_energy function with negative energy."""
    energy = -10.0  # MeV/u
    beta = beta_from_energy(energy)
    assert beta != beta, "Beta should be NaN for negative energy"

def test_beta_non_float_energy():
    """Test the beta_from_energy function with non-floating point energy."""
    energy = "100"  # Invalid type
    with pytest.raises(ValueError, match="Input must be a float, int, NumPy array, or a Python list of numbers"):
        beta_from_energy(energy)

def test_beta_increasing_energy():
    """Test that beta increases with increasing energy."""
    energies = [10.0, 50.0, 100.0, 500.0, 1000.0]  # MeV/u
    betas = [beta_from_energy(e) for e in energies]
    assert all(b1 < b2 for b1, b2 in zip(betas, betas[1:])), "Beta should increase with energy"

def test_beta_numpy_array():
    """Test the beta_from_energy function with a NumPy array."""
    energies = np.array([10.0, 50.0, 100.0, 500.0])  # MeV/u
    betas = beta_from_energy(energies)
    assert isinstance(betas, np.ndarray), "Output should be a NumPy array"
    assert betas.shape == energies.shape, "Output shape should match input shape"
    assert all(0.0 < b < 1.0 for b in betas), "All beta values should be between 0 and 1"

def test_beta_empty_numpy_array():
    """Test the beta_from_energy function with an empty NumPy array."""
    energies = np.array([])  # Empty array
    betas = beta_from_energy(energies)
    assert isinstance(betas, np.ndarray), "Output should be a NumPy array"
    assert betas.size == 0, "Output should be an empty array"

def test_beta_python_list():
    """Test the beta_from_energy function with a Python list."""
    energies = [10.0, 50.0, 100.0, 500.0]  # MeV/u
    betas = beta_from_energy(energies)
    assert isinstance(betas, list), "Output should be a Python list"
    assert len(betas) == len(energies), "Output length should match input length"
    assert all(0.0 < b < 1.0 for b in betas), "All beta values should be between 0 and 1"

def test_beta_empty_python_list():
    """Test the beta_from_energy function with an empty Python list."""
    energies = []  # Empty list
    betas = beta_from_energy(energies)
    assert isinstance(betas, list), "Output should be a Python list"
    assert len(betas) == 0, "Output should be an empty list"

def test_beta_invalid_numpy_array():
    """Test the beta_from_energy function with an invalid NumPy array."""
    energies = np.array(["invalid", "data"])  # Invalid types
    with pytest.raises(ValueError, match="Input must be a float, int, NumPy array, or a Python list of numbers"):
        beta_from_energy(energies)

def test_beta_invalid_python_list():
    """Test the beta_from_energy function with an invalid Python list."""
    energies = ["invalid", 100.0]  # Mixed types
    with pytest.raises(RuntimeError, match=r"Unable to cast Python instance of type <class 'str'> to C\+\+ type.*"):
        beta_from_energy(energies)

def test_beta_from_energy_60_MeV_u():
    """Test the beta_from_energy function for 60 MeV/u."""
    energy = 60.0  # MeV/u
    beta = beta_from_energy(energy)
    assert beta < 0.5, "Beta should be smaller than 0.5 for 60 MeV/u"