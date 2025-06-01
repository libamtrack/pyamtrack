import numpy as np
import pytest

from pyamtrack.converters import beta_from_energy, energy_from_beta


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
