#pragma once
#include "DataManager.h"

// Pure Win32 modal dialog for plugin options.
// Uses IDD_OPTIONS_DLG from the DLL's resource section.
bool ShowOptionsDialog(HWND hParent);
