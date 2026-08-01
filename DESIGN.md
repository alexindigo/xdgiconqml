# xdgiconqml — XDG Icon Theme QML Plugin

## What

A standalone QML module that provides XDG Icon Theme Specification (v0.13)
icon lookup to QML applications. Zero KDE dependencies — just `Qt6::Core`,
`Qt6::Qml`, and `Qt6::Quick`.

Pattern follows `mpvqml`, `dbusqml`, and `niriqml`: a thin C++ Qt6 QML plugin
that wraps standard Qt APIs into QML-friendly types.

## Why

QML applications on standalone window managers (Niri, Hyprland, etc.) need
icon lookup without pulling in KDE's `kiconthemes` or GNOME's `gtk3`.
`xdgiconqml` implements the spec directly using `QFileSystemWatcher` for
reactive theme updates.

## Module: `import XdgIcon 1.0`

## Types

### `XdgIcon`

Per-instance icon lookup element. Automatically resolves an icon name to
a file path based on the current theme and icon size. Re-resolves live
when the theme changes or new icons appear on disk.

**Properties:** `name`, `size`, `scale`, `themeOverride`, `path`, `found`,
`extension`, `isSymbolic`

**Methods:** `reload(force = false)`

### `XdgIconTheme`

Singleton providing theme management: current theme detection, available
themes listing, and change notifications. Access via attached properties
(e.g. `XdgIconTheme.currentTheme`).

**Properties:** `currentTheme`, `availableThemes`, `searchPaths`,
`dbusBroadcastEnabled`

**Methods:** `rescan()`

## Internal Architecture

```
XdgIcon ──→ XdgResolver (cache, theme chain, invalidation listeners)
              │
              ├── XdgLookup ──→ XdgIndexParse
              ├── XdgPathWatcher
              ├── XdgThemeWatcher
              └── XdgIconBroadcast (D-Bus, optional)
```

## D-Bus Integration

When built with `WITH_DBUS_BROADCAST=ON` and enabled at runtime via
`XdgIconTheme.dbusBroadcastEnabled`, the plugin listens for theme
changes via `org.freedesktop.portal.Settings` and participates in a
cross-process `org.atmosphera.IconResolver.IconChanged` broadcast.
Never a correctness dependency — filesystem watchers are the source
of truth.

## License

GPL-3.0-or-later
