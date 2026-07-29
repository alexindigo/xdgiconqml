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

XdgIconTheme {
    id: theme
    // currentTheme detected automatically
    // availableThemes provides a list
}
```

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

- Qt 6.8+ (Core, Qml, Quick)
- Optionally Qt 6.8+ (DBus) — enabled with `-DWITH_DBUS_BROADCAST=ON`

## License

GPL-3.0
