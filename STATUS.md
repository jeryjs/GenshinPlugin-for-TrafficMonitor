# Status: DONE ✅

## What was built
- **GenshinPlugin.dll** (64-bit, 1.8 MB) — compiled with GCC 13.2.0 (w64devkit)
- Pure Win32, no MFC dependency
- 3 taskbar items: Stamina, Realm Currency, Expeditions
- Background worker fetches API every 60s (configurable via Options)
- Options dialog with checkboxes + refresh interval spinner

## Where it's deployed
- Source: `Y:\Tools\TrafficMonitor\GenshinPlugin\`
- DLL: `Y:\Tools\TrafficMonitor\plugins\GenshinPlugin.dll`

## Config already updated
- `config.ini` → `plugin_display_item` includes `genshinstamina,genshinrealm,genshinexpedition`
- Display strings set for main window and taskbar

## What you need to do
1. Start TrafficMonitor normally (from your desktop)
2. Right-click the taskbar icon → **Other Functions → Plugin Management**
3. You should see **"Genshin Notes"** in the list
4. Enable it if not already enabled
5. Click **Options** to configure which items to show and refresh interval
6. The items will appear in your taskbar: `Stamina 139/200 | Realm 1620/2400 | Exp 4/5`

## Rebuild (if needed)
```bat
cd Y:\Tools\TrafficMonitor\GenshinPlugin
set PATH=C:\Users\jery\AppData\Local\Temp\w64devkit\w64devkit\bin;%PATH%
cmake -B build -G "MinGW Makefiles"
cmake --build build
copy GenshinPlugin.dll ..\plugins\
```

## INI config (auto-created on first run)
`GenshinPlugin.dll.ini` in TrafficMonitor's config dir:
```ini
[config]
refresh_interval=60
[display]
stamina=1
realm=1
expedition=1
```
