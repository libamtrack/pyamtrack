import numpy as np
import pytest

from pyamtrack.converters import beta_from_energy, energy_from_beta
from pyamtrack.stopping import electron_range

# Define test parameters: function, min value, max value
test_cases = [
    (beta_from_energy, 0, 1000),
    (energy_from_beta, 0, 1),
    (electron_range, 0, 1000),
]


@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_function_behavior(func, min_val, max_val):
    """
    Test the behavior of the provided function for various inputs.

    This includes:
    - Zero argument handling
    - List and numpy array input handling
    - Negative input handling
    - Non-numeric input handling
    - Monotonicity check
    """
    # Test: Function should return zero for zero argument
    assert func(0) == 0, f"{func.__name__} failed for zero argument."

    # Test: Function should accept list of floats and numpy array
    input_list = [min_val, (min_val + max_val) / 2, max_val]
    input_array = np.array(input_list)
    output_list = func(input_list)
    output_array = func(input_array)
    assert np.allclose(output_list, output_array), f"{func.__name__} failed for list and numpy array."
    assert isinstance(output_array, np.ndarray), f"{func.__name__} did not return a numpy array for numpy input."

    # Test: Function should return NaN or inf for negative numbers
    negative_input = -1 * (min_val + 1)
    result = func(negative_input)
    assert np.isnan(result) or np.isinf(result), f"{func.__name__} failed for negative input."

    # Test: Corner cases for non-numeric values
    with pytest.raises(TypeError):
        func("string")
    with pytest.raises(TypeError):
        func(None)
    with pytest.raises(TypeError):
        func([1, "string", 3])  # Mixture of numbers and non-numerical types

    # Test: Function should be monotonically increasing
    inputs = np.linspace(min_val, max_val, 100)
    outputs = func(inputs)
    assert np.all(np.diff(outputs) >= 0), f"{func.__name__} is not monotonically increasing."
