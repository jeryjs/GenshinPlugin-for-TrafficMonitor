#include "pch.h"
#include "PluginGenshin.h"
#include "DataManager.h"
#include "HttpClient.h"
#include "OptionsDlg.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

static bool LocalTimeSafe(std::tm& out, const std::time_t& value)
{
#if defined(_MSC_VER)
    return localtime_s(&out, &value) == 0;
#else
    if (std::tm* local = std::localtime(&value))
    {
        out = *local;
        return true;
    }
    return false;
#endif
}

// Helper to format current time for display
static std::wstring GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
    LocalTimeSafe(localTime, time);
    std::wostringstream oss;
    oss << std::put_time(&localTime, L"%Y-%m-%d %H:%M:%S");
    return oss.str();
}

CPluginGenshin CPluginGenshin::m_instance;

// ---------------------------------------------------------------------------
// Thread entry point (CreateThread requires a static/global function)
// ---------------------------------------------------------------------------

static DWORD WINAPI WorkerThreadProc(LPVOID lpParam)
{
    CPluginGenshin::Instance().WorkerLoop();
    return 0;
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

CPluginGenshin::CPluginGenshin()
{
    BuildActiveList();
}

CPluginGenshin::~CPluginGenshin()
{
    m_stop = true;
    if (m_hWorker)
    {
        WaitForSingleObject(m_hWorker, 5000);
        CloseHandle(m_hWorker);
        m_hWorker = NULL;
    }
}

CPluginGenshin& CPluginGenshin::Instance()
{
    return m_instance;
}

// ---------------------------------------------------------------------------
// Active-item list
// ---------------------------------------------------------------------------

void CPluginGenshin::BuildActiveList()
{
    m_activeCount = 0;
    if (g_data.m_showStamina)
        m_active[m_activeCount++] = &m_staminaItem;
    if (g_data.m_showRealm)
        m_active[m_activeCount++] = &m_realmItem;
    if (g_data.m_showExpedition)
        m_active[m_activeCount++] = &m_expeditionItem;
}

// ---------------------------------------------------------------------------
// ITMPlugin
// ---------------------------------------------------------------------------

IPluginItem* CPluginGenshin::GetItem(int index)
{
    if (index >= 0 && index < m_activeCount)
        return m_active[index];
    return nullptr;
}

void CPluginGenshin::DataRequired()
{
    UpdateCachedStrings();
}

void CPluginGenshin::UpdateCachedStrings()
{
    AutoLock lock(g_data.m_mutex);
    wchar_t buf[16];
    swprintf(buf, L"%d/%d", g_data.m_staminaCurrent, g_data.m_staminaMax);
    g_data.m_staminaText = buf;
    swprintf(buf, L"%d/%d", g_data.m_realmCurrent, g_data.m_realmMax);
    g_data.m_realmText = buf;
    swprintf(buf, L"%d/%d", g_data.m_expeditionFinished, g_data.m_expeditionTotal);
    g_data.m_expeditionText = buf;
}

const wchar_t* CPluginGenshin::GetInfo(PluginInfoIndex index)
{
    static wchar_t buf[256];
    HMODULE hMod = NULL;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(&CPluginGenshin::Instance),
        &hMod);

    switch (index)
    {
    case TMI_NAME:
        LoadStringW(hMod, IDS_PLUGIN_NAME, buf, _countof(buf));
        return buf;
    case TMI_DESCRIPTION:
        LoadStringW(hMod, IDS_PLUGIN_DESC, buf, _countof(buf));
        return buf;
    case TMI_AUTHOR:     return L"Jery";
    case TMI_COPYRIGHT:  return L"MIT License";
    case TMI_VERSION:    return L"1.0";
    case TMI_URL:        return L"https://github.com/zhongyang219/TrafficMonitor";
    default:             return L"";
    }
}

ITMPlugin::OptionReturn CPluginGenshin::ShowOptionsDialog(void* hParent)
{
    HWND hwnd = static_cast<HWND>(hParent);
    bool changed = ::ShowOptionsDialog(hwnd);
    if (changed)
        BuildActiveList();
    return changed ? OR_OPTION_CHANGED : OR_OPTION_UNCHANGED;
}

void CPluginGenshin::OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data)
{
    if (index == EI_CONFIG_DIR && data)
        LoadConfigOrDefaults(data);
}

void CPluginGenshin::OnInitialize(ITrafficMonitor* pApp)
{
    m_app = pApp;
    if (!m_configLoaded)
        LoadConfigOrDefaults(L"");

    if (!m_hWorker)
    {
        m_stop = false;
        m_hWorker = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    }
}

// ---------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------

void CPluginGenshin::LoadConfigOrDefaults(const wchar_t* configDir)
{
    if (m_configLoaded)
        return;
    g_data.LoadConfig(configDir ? configDir : L"");
    BuildActiveList();
    m_configLoaded = true;
}

const wchar_t* CPluginGenshin::GetTooltipInfo() {
    static wchar_t buf[256];
    AutoLock lock(g_data.m_mutex);
    if (g_data.m_lastRefreshTime.empty()) {
        swprintf(buf, L"Last refresh: never");
    } else {
        swprintf(buf, L"Last refresh: %s", g_data.m_lastRefreshTime.c_str());
    }
    return buf;
}

// Trigger an immediate refresh (called from options dialog)
void CPluginGenshin::TriggerRefresh() {
    // For now, just call FetchFromApi directly (it's thread-safe via mutex)
    FetchFromApi();
}

// ---------------------------------------------------------------------------
// Background worker
// ---------------------------------------------------------------------------

void CPluginGenshin::WorkerLoop()
{
    FetchFromApi();

    while (!m_stop)
    {
        int interval = g_data.m_refreshInterval;
        if (interval < 10) interval = 10;
        // Sleep in 1-second ticks for quick shutdown
        for (int i = 0; i < interval && !m_stop; ++i)
            Sleep(1000);
        if (!m_stop)
            FetchFromApi();
    }
}

void CPluginGenshin::FetchFromApi()
{
    std::wstring url;
    {
        AutoLock lock(g_data.m_mutex);
        url = g_data.m_apiUrl;
    }
    if (url.empty())
        return;

    std::string response = HttpGet(url);
    if (response.empty())
        return;

    try
    {
        json j = json::parse(response);
        if (!j.value("success", false))
            return;

        auto data = j["data"];
        if (!data.is_array() || data.empty())
            return;

        auto& acc = data[0];
        int stCur = 0, stMax = 0;
        int reCur = 0, reMax = 0;
        int expFin = 0, expTotal = 0;

        if (acc.contains("stamina") && acc["stamina"].is_object())
        {
            stCur = acc["stamina"].value("currentStamina", 0);
            stMax = acc["stamina"].value("maxStamina", 200);
        }

        if (acc.contains("realm") && acc["realm"].is_object())
        {
            reCur = acc["realm"].value("currentCoin", 0);
            reMax = acc["realm"].value("maxCoin", 2400);
        }

        if (acc.contains("expedition") && acc["expedition"].contains("list"))
        {
            auto& list = acc["expedition"]["list"];
            if (list.is_array())
            {
                expTotal = static_cast<int>(list.size());
                for (auto& e : list)
                {
                    if (e.value("status", std::string()) == "Finished")
                        ++expFin;
                }
            }
        }

        AutoLock lock(g_data.m_mutex);
        g_data.m_staminaCurrent = stCur;
        g_data.m_staminaMax = stMax;
        g_data.m_realmCurrent = reCur;
        g_data.m_realmMax = reMax;
        g_data.m_expeditionFinished = expFin;
        g_data.m_expeditionTotal = expTotal > 0 ? expTotal : 5;

        // Update cached values and last refresh timestamp on successful fetch
        g_data.m_cachedStaminaCurrent = stCur;
        g_data.m_cachedStaminaMax = stMax;
        g_data.m_cachedRealmCurrent = reCur;
        g_data.m_cachedRealmMax = reMax;
        g_data.m_cachedExpeditionFinished = expFin;
        g_data.m_cachedExpeditionTotal = expTotal > 0 ? expTotal : 5;
        g_data.m_lastRefreshTime = GetCurrentTimeString();

        // Persist cached values to disk
        g_data.SaveConfig();
    }
    catch (...)
    {
        // JSON parse error – keep last good values
    }
}

// ---------------------------------------------------------------------------
// DLL export
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) ITMPlugin* TMPluginGetInstance()
{
    return &CPluginGenshin::Instance();
}

#ifdef __cplusplus
}
#endif
