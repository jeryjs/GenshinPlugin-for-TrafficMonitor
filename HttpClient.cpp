#include "pch.h"
#include "HttpClient.h"
#include <wininet.h>

std::string HttpGet(const std::wstring& url, DWORD timeoutMs)
{
    std::string result;

    HINTERNET hInternet = InternetOpenW(
        L"GenshinPlugin/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL, NULL, 0);
    if (!hInternet)
        return result;

    // Set timeouts
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    HINTERNET hUrl = InternetOpenUrlW(
        hInternet, url.c_str(),
        NULL, 0,
        INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD,
        0);
    if (!hUrl)
    {
        InternetCloseHandle(hInternet);
        return result;
    }

    char buf[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hUrl, buf, sizeof(buf), &bytesRead) && bytesRead > 0)
    {
        result.append(buf, bytesRead);
        bytesRead = 0;
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return result;
}
