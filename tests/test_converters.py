import pytest
import numpy as np
from pyamtrack.converters import beta_from_energy, energy_from_beta, max_E_transfer_MeV


def test_beta_from_energy_60_MeV_u():
    """Test the beta_from_energy function for 60 MeV/u."""
    energy = 60.0  # MeV/u
    beta = beta_from_energy(energy)
    assert beta < 0.5, "Beta should be smaller than 0.5 for 60 MeV/u"

def test_energy_from_beta_single():
    """Test the energy_from_beta function with a single value."""
    beta = 0.5
    energy = energy_from_beta(beta)
    assert energy > 80.0, "Energy should be positive for valid beta"

def test_max_E_transfer_MeV():
    a = max_E_transfer_MeV(1, 2)
    b = max_E_transfer_MeV([1, 2], [2, 3])
    c = max_E_transfer_MeV(np.array([1, 2]), np.array([2, 3]))
    assert isinstance(a, float) or isinstance(a, int)
    assert isinstance(b, list)
    assert isinstance(c, np.ndarray)