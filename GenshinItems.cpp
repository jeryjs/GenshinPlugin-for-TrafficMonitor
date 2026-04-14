#include "pch.h"
#include "GenshinItems.h"
#include "DataManager.h"

const wchar_t* CPluginStamina::GetItemValueText() const
{
    return g_data.m_staminaText.c_str();
}

const wchar_t* CPluginRealm::GetItemValueText() const
{
    return g_data.m_realmText.c_str();
}

const wchar_t* CPluginExpedition::GetItemValueText() const
{
    return g_data.m_expeditionText.c_str();
}
