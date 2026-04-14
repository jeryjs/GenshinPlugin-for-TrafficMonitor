# Build Instructions

## With MSVC Build Tools x64

```bat
cd Y:\Tools\TrafficMonitor\plugins-src\GenshinPlugin
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmake -B build -G "NMake Makefiles"
cmake --build build --config Release
```

DLL output: `release\GenshinPlugin.dll`.

No MFC required — pure Win32.

## Deploy

Copy `release\GenshinPlugin.dll` → TrafficMonitor's `plugins\` folder.
Restart TM → Plugin Management → enable "Genshin Notes" → Options to configure.
