import os
import sys

# For Python 3.8+ add the directory containing the DLLs (this package directory)
if sys.version_info >= (3, 8):
    os.add_dll_directory(os.path.dirname(__file__))

# Import all symbols from the compiled module
from .pyamtrack import *
