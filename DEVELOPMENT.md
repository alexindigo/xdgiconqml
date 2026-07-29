# Development

## Prerequisites

- Qt 6.8+
- CMake 3.16+
- C++17 compiler

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
scripts/run-tests <test-name>
```

## Code Style

- `.clang-format` at project root — run before committing
- `.qmllint.ini` for QML files
- Follow patterns in `dbusqml`, `mpvqml`, and `niriqml` for consistency
