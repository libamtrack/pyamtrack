from __future__ import annotations
import os
import sys
import ctypes

if sys.platform == "win32":
    # Determine the directory where the package is installed.
    package_dir = os.path.abspath(os.path.dirname(__file__))
    
    # Add the package directory to the PATH so that the dependent DLL (amtrack.dll) can be found.
    os.environ["PATH"] = package_dir + os.pathsep + os.environ.get("PATH", "")
    
    # If available (Python 3.8+), also add it using os.add_dll_directory.
    try:
        os.add_dll_directory(package_dir)
    except AttributeError:
        pass

    # Optionally, explicitly load the DLL using ctypes to force its initialization.
    dll_path = os.path.join(package_dir, "amtrack.dll")
    try:
        ctypes.CDLL(dll_path)
    except OSError as e:
        # You might want to log or print the error if the DLL fails to load.
        print(f"Warning: failed to load amtrack.dll from {dll_path}: {e}")

from ._core import __doc__, __version__, calculate_velocity, beta_from_energy

__all__ = ["__doc__", "__version__", "calculate_velocity", "beta_from_energy"]
