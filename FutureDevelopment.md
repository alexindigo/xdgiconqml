# Future Development

## Deferred

- `XdgIconThemeModel` for theme enumeration (analogous to `NiriWindows` in niriqml)
- `icon-theme.cache` binary index consumption (optional speed boost)
- `.icon` sidecar metadata (spec §4: DisplayName, EmbeddedTextRectangle, AttachPoints)
- Dash-stripping name fallback (`foo-bar-baz` → `foo-bar` → `foo`)
- `Context=` filtering for directory selection
- `Hidden=true` honoring for theme selection UI
- Benchmark suite for icon resolution performance
- Sanitiser CI job (ASan/UBSan)
