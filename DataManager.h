#pragma once
#include <string>
#include <windows.h>

class CDataManager
{
public:
    static CDataManager& Instance();

    void LoadConfig(const std::wstring& config_dir);
    void SaveConfig();

    // Cached display text (populated by DataRequired)
    std::wstring m_staminaText;
    std::wstring m_realmText;
    std::wstring m_expeditionText;

    // Parsed numeric values (written by worker thread)
    CRITICAL_SECTION m_mutex;
    int m_staminaCurrent{ 0 };
    int m_staminaMax{ 200 };
    int m_realmCurrent{ 0 };
    int m_realmMax{ 2400 };
    int m_expeditionFinished{ 0 };
    int m_expeditionTotal{ 5 };

    // Settings
    int m_refreshInterval{ 60 };
    bool m_showStamina{ true };
    bool m_showRealm{ true };
    bool m_showExpedition{ true };

    std::wstring m_apiUrl{ L"https://jeryjs.dedyn.io/hla/api/genshin/notes" };
    std::wstring m_configPath;

private:
    CDataManager();
    ~CDataManager();
    CDataManager(const CDataManager&) = delete;
    CDataManager& operator=(const CDataManager&) = delete;
};

#define g_data CDataManager::Instance()

// RAII wrapper for CRITICAL_SECTION
class AutoLock {
    CRITICAL_SECTION* m_cs;
public:
    AutoLock(CRITICAL_SECTION& cs) : m_cs(&cs) { EnterCriticalSection(m_cs); }
    ~AutoLock() { LeaveCriticalSection(m_cs); }
    AutoLock(const AutoLock&) = delete;
    AutoLock& operator=(const AutoLock&) = delete;
};
