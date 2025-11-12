"""Shared pytest fixtures and utilities for pyamtrack tests."""

import pytest


@pytest.fixture
def electron_energy_MeV_low():
    """Fixture providing a low electron energy in MeV for tests."""
    return 100.0


@pytest.fixture
def electron_energy_MeV_high():
    """Fixture providing a high electron energy in MeV for tests."""
    return 1000.0
