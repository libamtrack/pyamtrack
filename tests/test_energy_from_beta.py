import pytest
import numpy as np
from pyamtrack.converters import energy_from_beta

def test_energy_from_beta_single():
    """Test the energy_from_beta function with a single value."""
    beta = 0.5
    energy = energy_from_beta(beta)
    assert energy > 0.0, "Energy should be positive for valid beta"

def test_energy_from_beta_numpy_array():
    """Test the energy_from_beta function with a NumPy array."""
    betas = np.array([0.1, 0.2, 0.3, 0.4])
    energies = energy_from_beta(betas)
    assert isinstance(energies, np.ndarray), "Output should be a NumPy array"
    assert energies.shape == betas.shape, "Output shape should match input shape"
    assert all(e > 0.0 for e in energies), "All energy values should be positive"

def test_energy_from_beta_list():
    """Test the energy_from_beta function with a Python list."""
    betas = [0.1, 0.2, 0.3, 0.4]
    energies = energy_from_beta(betas)
    assert isinstance(energies, list), "Output should be a Python list"
    assert len(energies) == len(betas), "Output length should match input length"
    assert all(e > 0.0 for e in energies), "All energy values should be positive"

def test_energy_from_beta_invalid_input():
    """Test the energy_from_beta function with invalid input."""
    with pytest.raises(ValueError, match="Input must be a float, int, NumPy array, or a Python list of numbers"):
        energy_from_beta("invalid")