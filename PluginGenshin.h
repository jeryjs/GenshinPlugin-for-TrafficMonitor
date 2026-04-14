#pragma once
#include "PluginInterface.h"
#include "GenshinItems.h"
#include <atomic>

class CPluginGenshin : public ITMPlugin
{
public:
    CPluginGenshin();
    virtual ~CPluginGenshin();

    static CPluginGenshin& Instance();
    CPluginGenshin(const CPluginGenshin&) = delete;
    CPluginGenshin& operator=(const CPluginGenshin&) = delete;

    // ITMPlugin overrides
    virtual IPluginItem* GetItem(int index) override;
    virtual void DataRequired() override;
    virtual OptionReturn ShowOptionsDialog(void* hParent) override;
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override;
    virtual void OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data) override;
    virtual void OnInitialize(ITrafficMonitor* pApp) override;
    virtual const wchar_t* GetTooltipInfo() override;

    // Called by the static thread entry point
    void WorkerLoop();

    // Used by the options dialog's Refresh now button
    void TriggerRefresh();

private:
    CPluginStamina      m_staminaItem;
    CPluginRealm        m_realmItem;
    CPluginExpedition   m_expeditionItem;

    IPluginItem* m_active[3];
    int m_activeCount{ 0 };

    ITrafficMonitor* m_app{ nullptr };
    HANDLE m_hWorker{ NULL };
    std::atomic_bool m_stop{ false };
    bool m_configLoaded{ false };

    void BuildActiveList();
    void LoadConfigOrDefaults(const wchar_t* configDir);
    void FetchFromApi();
    void UpdateCachedStrings();

    static CPluginGenshin m_instance;
};
