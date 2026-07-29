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
a file path based on the current theme and icon size.

**Properties:** `name`, `size`, `scale`, `themeOverride`, `path`, `found`

**Methods:** `reload()`

### `XdgIconTheme`

Singleton providing theme management: current theme detection, available
themes listing, and change notifications.

**Properties:** `currentTheme`, `availableThemes`

**Methods:** `rescan()`

## Internal Architecture

```
XdgIcon ──→ XdgLookup ──→ XdgIndexParse
              │
              ├── XdgCache
              ├── XdgPathWatcher
              ├── XdgThemeWatcher
              └── XdgBroadcast (D-Bus, optional)
```

## D-Bus Integration

When built with `WITH_DBUS_BROADCAST=ON`, the plugin listens for theme
changes via:
- `org.freedesktop.portal.Settings` (XDG Desktop Portal)
- `org.kde.GtkConfig` / GSettings (legacy)

## License

GPL-3.0
