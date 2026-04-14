#pragma once
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <string>
#include <atomic>
#include <chrono>
#include <vector>
#include "PluginInterface.h"
#include "resource.h"

// Portable replacements for MSVC-specific helpers
#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
