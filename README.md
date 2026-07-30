# xdgiconqml

Qt 6 QML plugin implementing the [XDG Icon Theme Specification](https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html) (v0.13) as a reactive QML library.

## QML Module

```
import XdgIcon 1.0
```

## Types

| Type | Description |
|------|-------------|
| `XdgIcon` | Per-instance icon lookup element |
| `XdgIconTheme` | Theme management singleton |

## Quick Start

```qml
import XdgIcon 1.0

XdgIcon {
    name: "firefox"
    size: 48
}

// XdgIconTheme is a singleton — access via attached properties:
Text { text: XdgIconTheme.currentTheme }
// XdgIconTheme.availableThemes provides a list of installed themes
```

## Features

- Full XDG Icon Theme Specification v0.13 lookup
- Reactive live invalidation via filesystem watchers
- Lazy per-theme index caching
- D-Bus broadcast support (optional, `WITH_DBUS_BROADCAST=ON`)
- GTK-parity permissive behaviours (no-`index.theme` trees, loose-file fallback)
- Debug CLI tool: `xdgiconqml-lookup`

## Building

```sh
# Release build
scripts/build

# Development build (with tests)
scripts/build-dev

# Run tests
scripts/run-tests
```

## Dependencies

- Qt 6.4+ (Core, Qml, Quick)
- Optionally Qt 6.4+ (DBus) — enabled with `-DWITH_DBUS_BROADCAST=ON`

## License

GPL-3.0-or-later
