import pytest
import pyamtrack


def test_beta_from_energy():
    """Test the beta_from_energy function."""
    energy = 100.0  # MeV/u
    beta = pyamtrack.beta_from_energy(energy)
    assert 0.0 < beta < 1.0, "Beta should be between 0 and 1"

# test zero energy
def test_beta_zero_energy():
    """Test the beta_from_energy function with zero energy."""
    energy = 0.0  # MeV/u
    beta = pyamtrack.beta_from_energy(energy)
    assert beta == 0.0, "Beta should be 0 for zero energy"


def test_beta_negative_energy():
    """Test the beta_from_energy function with negative energy."""
    energy = -10.0  # MeV/u
    # expect not-a-number (NaN) for negative energy
    beta = pyamtrack.beta_from_energy(energy)
    assert beta != beta, "Beta should be NaN for negative energy"

def test_beta_non_float_energy():
    """Test the beta_from_energy function with non-floating point energy."""
    energy = "100"  # Invalid type
    with pytest.raises(TypeError, match="beta_from_energy\\(\\): incompatible function arguments"):
        pyamtrack.beta_from_energy(energy)

def test_beta_increasing_energy():
    """Test that beta increases with increasing energy."""
    energies = [10.0, 50.0, 100.0, 500.0, 1000.0]  # MeV/u
    betas = [pyamtrack.beta_from_energy(e) for e in energies]
    assert all(b1 < b2 for b1, b2 in zip(betas, betas[1:])), "Beta should increase with energy"