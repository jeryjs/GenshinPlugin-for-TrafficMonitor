#pragma once
#include "PluginInterface.h"

class CPluginStamina : public IPluginItem
{
public:
    virtual const wchar_t* GetItemName() const override { return L"Stamina"; }
    virtual const wchar_t* GetItemId() const override { return L"genshinstamina"; }
    virtual const wchar_t* GetItemLableText() const override { return L"Stamina"; }
    virtual const wchar_t* GetItemValueText() const override;
    virtual const wchar_t* GetItemValueSampleText() const override { return L"200/200"; }
};

class CPluginRealm : public IPluginItem
{
public:
    virtual const wchar_t* GetItemName() const override { return L"Realm Currency"; }
    virtual const wchar_t* GetItemId() const override { return L"genshinrealm"; }
    virtual const wchar_t* GetItemLableText() const override { return L"Realm"; }
    virtual const wchar_t* GetItemValueText() const override;
    virtual const wchar_t* GetItemValueSampleText() const override { return L"2400/2400"; }
};

class CPluginExpedition : public IPluginItem
{
public:
    virtual const wchar_t* GetItemName() const override { return L"Expeditions"; }
    virtual const wchar_t* GetItemId() const override { return L"genshinexpedition"; }
    virtual const wchar_t* GetItemLableText() const override { return L"Expeditions"; }
    virtual const wchar_t* GetItemValueText() const override;
    virtual const wchar_t* GetItemValueSampleText() const override { return L"5/5"; }
};
