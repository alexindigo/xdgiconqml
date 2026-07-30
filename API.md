# XdgIcon QML API Reference

## Module: `import XdgIcon 1.0`

---

### `XdgIcon`

Per-instance icon lookup. Each `XdgIcon` element resolves an icon name
to a file path based on the current XDG icon theme. The path re-resolves
automatically when the theme changes or new icons appear on disk.

```qml
XdgIcon {
    name: "firefox"
    size: 48
    scale: 1
}
```

#### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `name` | `string` | `""` | Icon name (e.g. `"firefox"`, `"document-open"`) |
| `size` | `int` | `48` | Desired icon size in pixels |
| `scale` | `int` | `1` | Display scale factor |
| `themeOverride` | `string` | `""` | Override the current theme for this icon |
| `path` | `url` | `""` | Resolved icon file path (read-only) |
| `found` | `bool` | `false` | Whether the icon was found (read-only) |
| `extension` | `string` | `""` | File extension of the resolved icon, e.g. `"png"` (read-only) |
| `isSymbolic` | `bool` | `false` | True when the icon name ends with `-symbolic` (read-only) |

#### Signals

| Signal | Parameters | Emitted when |
|--------|------------|--------------|
| `nameChanged()` | — | `name` property changes |
| `sizeChanged()` | — | `size` property changes |
| `scaleChanged()` | — | `scale` property changes |
| `themeOverrideChanged()` | — | `themeOverride` property changes |
| `pathChanged()` | `newPath: string` | `path` property changes |
| `foundChanged()` | `found: bool` | `found` property changes |
| `extensionChanged()` | — | `extension` property changes |
| `isSymbolicChanged()` | — | `isSymbolic` property changes |

#### Methods

| Method | Description |
|--------|-------------|
| `reload(force: bool = false)` | Re-resolve the icon. With `force=true`, bypasses the cache. |

---

### `XdgIconTheme`

Singleton providing theme management. Access via attached properties —
do not instantiate.

```qml
Text { text: XdgIconTheme.currentTheme }
```

#### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `currentTheme` | `string` | auto-detected | Current icon theme name (read/write) |
| `availableThemes` | `string[]` | `[]` | List of installed themes (read-only) |
| `searchPaths` | `string[]` | auto-detected | Icon search directories in order (read-only) |
| `dbusBroadcastEnabled` | `bool` | `false` | Enable D-Bus broadcast (read/write; requires `WITH_DBUS_BROADCAST` build) |

#### Theme discovery order

On first construction, `currentTheme` is auto-detected in this order:

1. `QS_ICON_THEME` environment variable
2. `QT_QPA_PLATFORMTHEME`'s platform config (gtk3, gtk4, qt6ct, or kde)
3. GTK 3 / GTK 4 `settings.ini` (in order)
4. qt6ct `qt6ct.conf`
5. kdeglobals `[Icons]/Theme`
6. Fallback to `"hicolor"`

#### Signals

| Signal | Emitted when |
|--------|--------------|
| `currentThemeChanged()` | `currentTheme` property changes |
| `availableThemesChanged()` | `availableThemes` list changes |
| `searchPathsChanged()` | `searchPaths` list changes |
| `themeReloaded()` | Theme data has been reloaded after a rescan |
| `propertiesChanged()` | Any property may have changed (after external events) |

#### Methods

| Method | Description |
|--------|-------------|
| `rescan()` | Re-scan available themes from XDG data directories |
