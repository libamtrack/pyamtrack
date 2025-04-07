import pytest
import numpy as np
from pyamtrack.converters import beta_from_energy, energy_from_beta

# Define test parameters: function, min value, max value
test_cases = [
    (beta_from_energy, 0, 1000),
    (energy_from_beta, 0, 1),
]

@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_zero_argument(func, min_val, max_val):
    # Test: Function should return zero for zero argument
    assert func(0) == 0, f"{func.__name__} failed for zero argument."

@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_accept_list_and_numpy(func, min_val, max_val):
    # Test: Function should accept list of floats and numpy array
    input_list = [min_val, (min_val + max_val) / 2, max_val]
    input_array = np.array(input_list)
    output_list = func(input_list)
    output_array = func(input_array)
    assert np.allclose(output_list, output_array), f"{func.__name__} failed for list and numpy array."
    assert isinstance(output_array, np.ndarray), f"{func.__name__} did not return a numpy array for numpy input."

@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_negative_input(func, min_val, max_val):
    # Test: Function should return NaN or inf for negative numbers
    negative_input = -1 * (min_val + 1)
    result = func(negative_input)
    assert np.isnan(result) or np.isinf(result), f"{func.__name__} failed for negative input."

@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_non_numeric_values(func, min_val, max_val):
    # Test: Corner cases for non-numeric values
    with pytest.raises(ValueError):
        func("string")
    with pytest.raises(ValueError):
        func(None)
    with pytest.raises(RuntimeError):
        func([1, "string", 3])  # Mixture of numbers and non-numerical types

@pytest.mark.parametrize("func, min_val, max_val", test_cases)
def test_monotonicity(func, min_val, max_val):
    # Test: Function should be monotonically increasing
    inputs = np.linspace(min_val, max_val, 100)
    outputs = func(inputs)
    assert np.all(np.diff(outputs) >= 0), f"{func.__name__} is not monotonically increasing."
