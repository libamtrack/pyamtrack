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


def assert_list_of_type(result, expected_type, expected_first_value=None):
    """
    Helper function to validate a list returned by getter functions.
    
    Args:
        result: The list to validate
        expected_type: The expected type of list elements (e.g., int, str)
        expected_first_value: Optional expected value of the first element
    """
    assert isinstance(result, list)
    assert len(result) > 0
    assert all(isinstance(item, expected_type) for item in result)
    if expected_first_value is not None:
        assert result[0] == expected_first_value
