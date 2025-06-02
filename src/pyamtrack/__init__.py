from __future__ import annotations

import ctypes
import os
import sys

try:
    import pyamtrack._version as v

    __version__ = v.version
except ImportError:
    __version__ = "0.0.0"  # Fallback version


# Determine the directory where the pyamtrack package is installed.
package_dir = os.path.abspath(os.path.dirname(__file__))
# print("pyamtrack package directory:", package_dir)
# print("Contents of pyamtrack package directory:", os.listdir(package_dir))

if sys.platform == "win32":
    # Ensure the package directory is in the PATH for DLL lookup.
    os.environ["PATH"] = package_dir + os.pathsep + os.environ.get("PATH", "")
    try:
        os.add_dll_directory(package_dir)
    except AttributeError:
        # Older Python versions may not have os.add_dll_directory.
        pass

    # List of DLLs that _core depends on.
    dependent_dlls = ["amtrack.dll", "gsl.dll", "gslcblas.dll"]
    for dll_name in dependent_dlls:
        dll_path = os.path.join(package_dir, dll_name)
        try:
            ctypes.CDLL(dll_path)
            # print(f"Successfully loaded {dll_name} from {dll_path}")
        except OSError as e:
            print(f"Warning: failed to load {dll_name} from {dll_path}: {e}")


from . import converters, materials, stopping

__all__ = ["converters", "stopping", "materials"]
