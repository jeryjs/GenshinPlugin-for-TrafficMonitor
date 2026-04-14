#include "pch.h"
#include "DataManager.h"

CDataManager::CDataManager()
{
    InitializeCriticalSection(&m_mutex);
}

CDataManager::~CDataManager()
{
    DeleteCriticalSection(&m_mutex);
}

CDataManager& CDataManager::Instance()
{
    static CDataManager instance;
    return instance;
}

static HMODULE GetCurrentModule()
{
    HMODULE hModule = NULL;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(&CDataManager::Instance),
        &hModule);
    return hModule;
}

static void WriteInt(LPCWSTR section, LPCWSTR key, int value, LPCWSTR path)
{
    wchar_t buf[16];
    swprintf(buf, L"%d", value);
    WritePrivateProfileStringW(section, key, buf, path);
}

void CDataManager::LoadConfig(const std::wstring& config_dir)
{
    if (config_dir.empty())
        return;

    wchar_t modulePath[MAX_PATH];
    GetModuleFileNameW(GetCurrentModule(), modulePath, MAX_PATH);

    std::wstring dir = config_dir;
    if (dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');

    size_t pos = std::wstring(modulePath).find_last_of(L"\\/");
    std::wstring moduleName = (pos == std::wstring::npos)
        ? modulePath
        : std::wstring(modulePath).substr(pos + 1);

    m_configPath = dir + moduleName + L".ini";

    m_refreshInterval = GetPrivateProfileIntW(L"config", L"refresh_interval", m_refreshInterval, m_configPath.c_str());
    if (m_refreshInterval < 10) m_refreshInterval = 10;
    if (m_refreshInterval > 3600) m_refreshInterval = 3600;

    m_showStamina = GetPrivateProfileIntW(L"display", L"stamina", m_showStamina ? 1 : 0, m_configPath.c_str()) != 0;
    m_showRealm = GetPrivateProfileIntW(L"display", L"realm", m_showRealm ? 1 : 0, m_configPath.c_str()) != 0;
    m_showExpedition = GetPrivateProfileIntW(L"display", L"expedition", m_showExpedition ? 1 : 0, m_configPath.c_str()) != 0;
}

void CDataManager::SaveConfig()
{
    if (m_configPath.empty())
        return;

    WriteInt(L"config", L"refresh_interval", m_refreshInterval, m_configPath.c_str());
    WriteInt(L"display", L"stamina", m_showStamina ? 1 : 0, m_configPath.c_str());
    WriteInt(L"display", L"realm", m_showRealm ? 1 : 0, m_configPath.c_str());
    WriteInt(L"display", L"expedition", m_showExpedition ? 1 : 0, m_configPath.c_str());
}
