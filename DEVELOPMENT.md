# Development

## Prerequisites

- Qt 6.4+
- CMake 3.16+
- C++17 compiler
- clang-format (for code style enforcement)

## Building

```sh
# Development build with tests
scripts/build-dev

# Release build
scripts/build

# Clean everything
scripts/clean
```

## Running Tests

```sh
# Build dev first
scripts/build-dev

# Run all tests
scripts/run-tests

# Run specific test
scripts/run-tests -R <test-name>
```

## Code Style

- `scripts/run-clang-format-check` — verify formatting (pre-commit hook runs this)
- `scripts/run-qmllint` — lint QML examples
- Follow patterns in `dbusqml`, `mpvqml`, and `niriqml` for consistency
