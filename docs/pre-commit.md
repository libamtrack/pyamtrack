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
