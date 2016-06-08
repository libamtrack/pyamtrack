import unittest

from pyamtrack import run_pyamtrack


class TestFunMethod(unittest.TestCase):
    def test_momentum(self):
        m = run_pyamtrack.get_momentum(10.0)
        self.assertGreater(m, 0.0)


if __name__ == '__main__':
    unittest.main()
