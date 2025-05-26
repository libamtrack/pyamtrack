# Building and Installing `pyamtrack` on Linux

## **Prerequisites**
Ensure you have the following installed:
- **Python 3** (with development headers, at least 3.9)
- **CMake** (3.16 or later)
- **Ninja** (recommended for faster builds)
- **GSL** (GNU Scientific Library)
- **Git** (for fetching dependencies)

## **Installation Instructions**

Follow these steps to build and install the `pyamtrack` Python module as a wheel package.

### **Building and Installing the Project Locally**

To build and install the project locally, follow these steps:

1. **Create a Virtual Environment**:
   Create and activate a Python virtual environment:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```

2. **Install Development Dependencies**:
   Install the required dependencies for building the project:
   ```bash
   pip install -r requirements-dev.txt
   ```

3. **Build the Project**:
   Run the following command to build the project:
   ```bash
   python -m build --wheel --no-isolation --config-setting=build-dir=./build
   ```

   This command speeds up the build process for local development by avoiding recompilation of the `libamtrack` source code in every iteration.

4. **Install the Built Wheel**:
   After building, install the wheel:
   ```bash
   pip install dist/*.whl
   ```

5. **Test the Installation**:
   Verify the installation by running:
   ```bash
   python -c "import pyamtrack; print(pyamtrack.converters.beta_from_energy(150))"
   ```

Expected output:
```
Python bindings for libamtrack
```

### Using the `scripts/build_linux.sh` Script

Alternatively, you can use the `scripts/build_linux.sh` script to perform the same steps as above. This script automates the process of creating a virtual environment, installing dependencies, building the project, and testing the installation:

```bash
./scripts/build_linux.sh
```

### Code Formatting and Pre-commit Hooks

This project uses [pre-commit](https://pre-commit.com) to ensure code quality and consistent formatting.

To set it up:

1. **Create a Virtual Environment**:
   Create and activate a Python virtual environment:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```

2. **Install Development Dependencies**:
   Install the required dependencies for building the project:
   ```bash
   pip install -r requirements-dev.txt
   ```

3. **Install the Git hooks**:

   ```bash
   pre-commit install
   ```

4. **Run all hooks on all files once** (recommended after cloning the repo):

   ```bash
   pre-commit run --all-files
   ```

To update the hooks to their latest versions later:

```bash
pre-commit autoupdate
```

The following hooks are used:

* `black` for Python formatting
* `clang-format` for C++ formatting
* `trailing-whitespace` to remove trailing spaces
* `end-of-file-fixer` to ensure files end with a single newline
* `check-added-large-files` to avoid accidentally committing large binaries

All configuration is in `.pre-commit-config.yaml`. The hooks will run automatically every time you commit.

### **Uninstallation**
### **Remove the Installed Package**
```sh
pip uninstall pyamtrack
```

## **Rebuilding the Package**
### **Clean Previous Builds**
```sh
rm -rf build dist
```

### **Rebuild the Wheel**
```sh
python -m build --wheel
```

Now you can use `pyamtrack` in your Python scripts:
```python
import pyamtrack
print(pyamtrack.converters.beta_from_energy(150))
```

### Building with `cibuildwheel`

For a clean and isolated build environment, you can use `cibuildwheel`. This ensures that the compilation happens in a very clean environment, which is ideal for producing distributable wheels:

```sh
pip install cibuildwheel
```

Then, run the following command to build the wheel for a specific Python version and platform:

```bash
cibuildwheel --only "cp312-manylinux_x86_64"
```

To test the installed package, type in the virtual environment:

```bash
pip install wheelhouse/*whl
```

Then:

```python
>>> import pyamtrack
>>> pyamtrack.stopping.electron_range(120)
0.0006700734300203331
>>> pyamtrack.__version__
'0.14.1'
>>>
```
