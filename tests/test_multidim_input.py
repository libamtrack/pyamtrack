from random import uniform

import numpy as np
import pytest

from pyamtrack.converters import beta_from_energy, energy_from_beta
from pyamtrack.stopping import electron_range

zero_dim_cases = [
    (beta_from_energy, 0, 100),
    (energy_from_beta, 0, 1),
]

one_dim_cases = [
    (beta_from_energy, 0, 100),
    (energy_from_beta, 0, 1),
    (electron_range, 0, 1000),
]

# Create test cases for dimensions 2 through 5
multidim_base_cases = [(beta_from_energy, 0, 100), (energy_from_beta, 0, 1)]
multidim_cases = [base_args + (dim,) for base_args in multidim_base_cases for dim in range(2, 6)]


@pytest.mark.parametrize("func, min_val, max_val", zero_dim_cases)
def test_zero_dim_array(func, min_val, max_val):
    """
    Test that the function correctly handles 0-dimensional (scalar) NumPy inputs.
    The result from passing a 0-D NumPy array should match the result from a plain scalar input.
    """

    scalar = uniform(min_val, max_val)
    result_scalar = func(scalar)

    np_scalar = np.array(scalar)
    result_np_scalar = func(np_scalar)

    assert np_scalar.shape == result_np_scalar.shape
    assert np.allclose(result_scalar, result_np_scalar)


@pytest.mark.parametrize("func, min_val, max_val", one_dim_cases)
def test_one_dim_array(func, min_val, max_val):
    """
    Test that the function correctly handles 1-dimensional array inputs.
    The result from passing a list should match the result from a 1-D NumPy array with the same values.
    """

    one_dim_list = [uniform(min_val, max_val) for _ in range(100)]
    np_array = np.array(one_dim_list)

    np_result = func(np_array)

    assert np_result.shape == np_array.shape
    assert np.allclose(np.array(func(one_dim_list)), np_result)


@pytest.mark.parametrize("func, min_val, max_val, dim", multidim_cases)
def test_multi_dim_array(func, min_val, max_val, dim):
    """
    Test that the function correctly handles multi-dimensional array inputs.

    The result from applying the function to a higher-dimensional array should match the result
    obtained by applying the function along the leading axis (recursive validation).
    """

    array = np.random.uniform(low=min_val, high=max_val, size=(10,) * dim)

    assert len(array.shape) == dim
    assert array.size == 10**dim

    partial_results = []
    for lower_dim_arr in array:
        partial_results.append(func(lower_dim_arr))

    whole_results_from_partial = np.stack(partial_results)
    whole_results = func(array)

    assert array.shape == whole_results.shape
    assert np.allclose(whole_results_from_partial, whole_results)
