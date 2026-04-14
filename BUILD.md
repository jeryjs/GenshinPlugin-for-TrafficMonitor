# Build Instructions

## With MinGW (current system)

```bat
cd Y:\Tools\TrafficMonitor\GenshinPlugin
cmake -B build -G "MinGW Makefiles"
cmake --build build --config Release
```

DLL output: `GenshinPlugin.dll` in the project root.

## With Visual Studio (MSVC)

```bat
cd Y:\Tools\TrafficMonitor\GenshinPlugin
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

No MFC required — pure Win32.

## Deploy

Copy `GenshinPlugin.dll` → TrafficMonitor's `plugins\` folder.
Restart TM → Plugin Management → enable "Genshin Notes" → Options to configure.
