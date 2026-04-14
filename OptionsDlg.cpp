#include "pch.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "PluginGenshin.h"
#include <commctrl.h>

// MinGW headers may not define these extended updown messages
#ifndef UDM_SETRANGE32
#define UDM_SETRANGE32 (WM_USER + 111)
#endif
#ifndef UDM_SETPOS32
#define UDM_SETPOS32 (WM_USER + 113)
#endif

// Module handle – set from DllMain or first dialog call
static HMODULE g_hModule = NULL;

static HMODULE GetThisModule()
{
    if (!g_hModule)
    {
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(&ShowOptionsDialog),
            &g_hModule);
    }
    return g_hModule;
}

static INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        // Set initial checkbox states from current config
        CheckDlgButton(hDlg, IDC_CHK_STAMINA,
            g_data.m_showStamina ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHK_REALM,
            g_data.m_showRealm ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHK_EXPEDITION,
            g_data.m_showExpedition ? BST_CHECKED : BST_UNCHECKED);

        // Set spin range and position
        HWND hSpin = GetDlgItem(hDlg, IDC_SPIN_INTERVAL);
        SendMessage(hSpin, UDM_SETRANGE32, 10, 3600);
        SendMessage(hSpin, UDM_SETPOS32, 0, g_data.m_refreshInterval);

        return TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            // Validate interval
            BOOL ok = FALSE;
            int interval = GetDlgItemInt(hDlg, IDC_EDIT_INTERVAL, &ok, FALSE);
            if (!ok || interval < 10 || interval > 3600)
            {
                MessageBoxW(hDlg, L"Refresh interval must be 10-3600 seconds.",
                    L"Genshin Impact", MB_ICONWARNING);
                return TRUE;
            }

            // Write settings back
            g_data.m_showStamina = (IsDlgButtonChecked(hDlg, IDC_CHK_STAMINA) == BST_CHECKED);
            g_data.m_showRealm = (IsDlgButtonChecked(hDlg, IDC_CHK_REALM) == BST_CHECKED);
            g_data.m_showExpedition = (IsDlgButtonChecked(hDlg, IDC_CHK_EXPEDITION) == BST_CHECKED);
            g_data.m_refreshInterval = interval;
            g_data.SaveConfig();

            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        if (LOWORD(wParam) == IDC_BTN_REFRESH) {
            CPluginGenshin::Instance().TriggerRefresh();
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

bool ShowOptionsDialog(HWND hParent)
{
    HMODULE hMod = GetThisModule();
    INT_PTR result = DialogBoxParamW(
        hMod,
        MAKEINTRESOURCEW(IDD_OPTIONS_DLG),
        hParent,
        OptionsDlgProc,
        0);
    return (result == IDOK);
}
