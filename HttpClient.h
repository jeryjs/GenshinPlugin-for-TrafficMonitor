#pragma once
#include <string>

std::string HttpGet(const std::wstring& url, DWORD timeoutMs = 15000);
