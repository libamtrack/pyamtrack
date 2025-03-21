from __future__ import annotations

import os
import sys

# On Windows, add the package directory to the DLL search path so that dependent DLLs
# (such as amtrack.dll) can be located.
if sys.platform == "win32":
    package_dir = os.path.abspath(os.path.dirname(__file__))
    try:
        # Python 3.8+ provides os.add_dll_directory to add directories for DLL lookup.
        os.add_dll_directory(package_dir)
    except AttributeError:
        # For older Python versions, fallback by manually loading the DLL.
        from ctypes import WinDLL
        WinDLL(os.path.join(package_dir, "amtrack.dll"))

from ._core import __doc__, __version__, calculate_velocity, beta_from_energy

__all__ = ["__doc__", "__version__", "calculate_velocity", "beta_from_energy"]
