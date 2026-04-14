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

static int ReadInt(LPCWSTR section, LPCWSTR key, int defaultVal, LPCWSTR path) { return GetPrivateProfileIntW(section, key, defaultVal, path); }

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

    // Load cached last known values
    m_cachedStaminaCurrent = ReadInt(L"cached", L"stamina_current", m_cachedStaminaCurrent, m_configPath.c_str());
    m_cachedStaminaMax = ReadInt(L"cached", L"stamina_max", m_cachedStaminaMax, m_configPath.c_str());
    m_cachedRealmCurrent = ReadInt(L"cached", L"realm_current", m_cachedRealmCurrent, m_configPath.c_str());
    m_cachedRealmMax = ReadInt(L"cached", L"realm_max", m_cachedRealmMax, m_configPath.c_str());
    m_cachedExpeditionFinished = ReadInt(L"cached", L"expedition_finished", m_cachedExpeditionFinished, m_configPath.c_str());
    m_cachedExpeditionTotal = ReadInt(L"cached", L"expedition_total", m_cachedExpeditionTotal, m_configPath.c_str());

    // Initialize current values from cached values (fallback instead of 0)
    m_staminaCurrent = m_cachedStaminaCurrent;
    m_staminaMax = m_cachedStaminaMax;
    m_realmCurrent = m_cachedRealmCurrent;
    m_realmMax = m_cachedRealmMax;
    m_expeditionFinished = m_cachedExpeditionFinished;
    m_expeditionTotal = m_cachedExpeditionTotal;
}

void CDataManager::SaveConfig()
{
    if (m_configPath.empty())
        return;

    WriteInt(L"config", L"refresh_interval", m_refreshInterval, m_configPath.c_str());
    WriteInt(L"display", L"stamina", m_showStamina ? 1 : 0, m_configPath.c_str());
    WriteInt(L"display", L"realm", m_showRealm ? 1 : 0, m_configPath.c_str());
    WriteInt(L"display", L"expedition", m_showExpedition ? 1 : 0, m_configPath.c_str());

    // Save cached last known values
    WriteInt(L"cached", L"stamina_current", m_cachedStaminaCurrent, m_configPath.c_str());
    WriteInt(L"cached", L"stamina_max", m_cachedStaminaMax, m_configPath.c_str());
    WriteInt(L"cached", L"realm_current", m_cachedRealmCurrent, m_configPath.c_str());
    WriteInt(L"cached", L"realm_max", m_cachedRealmMax, m_configPath.c_str());
    WriteInt(L"cached", L"expedition_finished", m_cachedExpeditionFinished, m_configPath.c_str());
    WriteInt(L"cached", L"expedition_total", m_cachedExpeditionTotal, m_configPath.c_str());
}
