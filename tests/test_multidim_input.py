from random import uniform

import numpy as np
import pytest

from pyamtrack.stopping import electron_range


one_dim_cases = [
    (electron_range, 0.0, 1000.0),
]


@pytest.mark.parametrize("func, min_val, max_val", one_dim_cases)
def test_one_dim_array(func, min_val, max_val):
    """
    Test that electron_range correctly handles one-dimensional array inputs.

    The result from passing a list should match the result from passing a
    NumPy array with the same values.
    """

    one_dim_list = [uniform(min_val, max_val) for _ in range(100)]
    np_array = np.array(one_dim_list)

    np_result = func(np_array)

    assert np_result.shape == np_array.shape
    assert np.allclose(np.array(func(one_dim_list)), np_result)