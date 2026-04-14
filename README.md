# GenshinPlugin for TrafficMonitor

Shows **Stamina**, **Realm Currency**, and **Expedition** count from Genshin Impact
in your Windows taskbar via TrafficMonitor.

![display concept] Stamina 139/200 · Realm 1620/2400 · Expeditions 4/5

## What it does

- Fetches real-time notes from the HoyoLAB API every N seconds (configurable, default 60s)
- Displays each stat as a separate taskbar item with label + value
- Lets you pick which items to show and the refresh interval via Options dialog
- Saves settings to INI — survives restarts

## Quick build

```bat
cd Y:\Tools\TrafficMonitor\plugins-src\GenshinPlugin
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmake -B build -G "NMake Makefiles"
cmake --build build --config Release
```

> Needs the Visual Studio Build Tools x64 environment. See [BUILD.md](BUILD.md) for full instructions.

## Deploy

1. Copy `build\Release\GenshinPlugin.dll` → TrafficMonitor's `plugins\` folder
2. Restart TrafficMonitor
3. Right-click taskbar icon → Plugin Management → enable "Genshin Notes"
4. Click **Options** to configure

## Project layout

```
GenshinPlugin/
├── PluginGenshin.cpp/h    Main ITMPlugin, background worker, JSON parsing
├── GenshinItems.cpp/h     Three IPluginItem classes (Stamina, Realm, Expedition)
├── DataManager.cpp/h      Thread-safe cache + INI config persistence
├── HttpClient.cpp/h       WinHTTP GET helper
├── OptionsDlg.cpp/h       MFC options dialog (checkboxes + refresh spinner)
├── GenshinPlugin.rc       Dialog + string table resources
├── resource.h             Resource IDs
├── pch.h                  Precompiled header (MFC)
├── CMakeLists.txt         Build config
└── include/
    ├── PluginInterface.h  TrafficMonitor plugin API (from upstream)
    └── nlohmann/json.hpp  JSON parser (header-only library)
```

## Options dialog

Right-aligned checkboxes keep things compact:

```
    ┌─ Genshin Impact ────────────────────┐
    │  [✓] Stamina                        │
    │  [✓] Realm currency                 │
    │  [✓] Expeditions                    │
    │  Refresh every [60] ↑↓ seconds      │
    │                           [OK] [Cancel]
    └─────────────────────────────────────┘
```

## Config file (auto-generated)

`GenshinPlugin.dll.ini` in TrafficMonitor's config dir:

```ini
[config]
refresh_interval=60

[display]
stamina=1
realm=1
expedition=1
```
