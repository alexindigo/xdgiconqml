# XdgIcon QML API Reference

## Module: `import XdgIcon 1.0`

---

### `XdgIcon`

Per-instance icon lookup. Each `XdgIcon` element resolves an icon name
to a file path based on the current XDG icon theme.

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

#### Signals

| Signal | Emitted when |
|--------|--------------|
| `nameChanged()` | `name` property changes |
| `sizeChanged()` | `size` property changes |
| `scaleChanged()` | `scale` property changes |
| `themeOverrideChanged()` | `themeOverride` property changes |
| `pathChanged()` | `path` property changes |
| `foundChanged()` | `found` property changes |

#### Methods

| Method | Description |
|--------|-------------|
| `reload()` | Force re-resolve the icon |

---

### `XdgIconTheme`

Singleton providing theme management.

```qml
XdgIconTheme {
    id: theme
    onCurrentThemeChanged: console.log("Theme:", currentTheme)
}
```

#### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `currentTheme` | `string` | `""` | Current icon theme name |
| `availableThemes` | `string[]` | `[]` | List of installed themes (read-only) |

#### Signals

| Signal | Emitted when |
|--------|--------------|
| `currentThemeChanged()` | `currentTheme` property changes |
| `availableThemesChanged()` | `availableThemes` list changes |
| `themeReloaded()` | Theme data has been reloaded |

#### Methods

| Method | Description |
|--------|-------------|
| `rescan()` | Re-scan available themes from XDG data directories |
