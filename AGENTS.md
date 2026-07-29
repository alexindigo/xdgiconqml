# Agent Rules for xdgiconqml

## Build & Test

- Run `scripts/build-dev` before committing to verify the build.
- Run `scripts/run-tests` to execute C++ tests.
- Run `qmllint` on any changed `.qml` files.
- After QML changes, run a cold-load smoke test: `qs -c <shell> -d` expecting exit code 0.

## Code Style

- Follow `.clang-format` for C++ files.
- Use 4-space indentation, no tabs.
- Use Qt 6 signal/slot syntax (PMF style).
- Prefix private members with `m_`.

## Architecture

- Flat source layout (no `src/` subdirectory).
- Each class gets its own `.h`/`.cpp` pair.
- Internal implementation classes go in `.cpp` files, not separate headers.
- D-Bus broadcast code is gated behind `WITH_DBUS_BROADCAST`.
