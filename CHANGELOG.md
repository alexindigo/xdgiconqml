# Changelog

## v0.1.0 — First functional release

### Features

- Full XDG Icon Theme Specification v0.13 lookup (theme chain, directory
  scoring, exact-match → closest-size → unthemed fallback)
- Reactive live invalidation via filesystem watchers and 5-second mtime
  rescan timer
- `XdgResolver` engine + `XdgIconTheme` facade architecture
- Lazy per-theme `index.theme` caching
- D-Bus broadcast support (CMake-gated `WITH_DBUS_BROADCAST`; opt-in at
  runtime via `XdgIconTheme.dbusBroadcastEnabled`)
- GTK-parity permissive behaviours: no-`index.theme` tree walking,
  recursive fallback directory scan, loose-file fallback
- Theme detection from `QS_ICON_THEME`, `QT_QPA_PLATFORMTHEME`, GTK,
  qt6ct, and kdeglobals
- Search paths include `$HOME/.icons`, `/usr/local/share/icons`,
  `/usr/share/pixmaps`, NixOS, and Flatpak directories
- Debug CLI tool: `xdgiconqml-lookup`
- QML examples: `IconLookup.qml`, `AppIconGrid.qml`, `ThemeSwitcher.qml`

### Fixes

- Noctalia #2476 — runtime-materialised icons detected live without
  shell restart
- Noctalia #3379 (kew) — icons in trees without `index.theme` at any
  arbitrary size are correctly resolved
- Noctalia #3379 (pt2-clone) — bare loose-file icons resolved via
  unthemed fallback

### Quality

- 5 C++ test suites (50+ cases); integration tests against
  `QTemporaryDir` fixtures
- CI matrix: `WITH_DBUS_BROADCAST=ON`/`OFF`, clang-format, qmllint
- Pre-commit hook blocks formatting violations
- `.clang-format` enforced across all C++ sources

## v0.0.0 — Boilerplate

- Repository initialized with flat-source structure
- CMake build system with `qt_add_qml_module`
- Stub source files for all planned types
- Shell scripts for build/test/install
- CI workflow, `.gitignore`, `.clang-format`, `.qmllint.ini`
- Documentation stubs
- GPL-3.0 license
