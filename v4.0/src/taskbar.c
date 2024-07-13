#include "taskbar.h"
#include "mouse_wrap.h"
#include <windows.h>

#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXITAPP 1001
#define IDM_BUYMEACOFFEE 1002
#define IDM_TOGGLE_STARTUP 1003
#define IDM_VERSIONCHECK 1004

HINSTANCE hInstMain;

static NOTIFYICONDATA nid;
static HMENU hMenu;

BOOL IsDarkTheme()
{
    HKEY hKey;
    DWORD dwType = REG_DWORD;
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, L"AppsUseLightTheme", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return dwValue == 0; // 0 means dark theme
        }
        RegCloseKey(hKey);
    }
    return FALSE; // Default to light theme if there is an issue
}

void CreateTrayIcon(HWND hwnd, HINSTANCE hInst)
{
    hInstMain = hInst;

    BOOL isDarkTheme = IsDarkTheme();

    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(isDarkTheme ? IDI_ICON_ENABLED_DARK : IDI_ICON_ENABLED_LIGHT));

    //#define TITLE_VERSION WINDOW_TITLE L" " WPRODVER_STR
    wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t), WINDOW_TITLE L" " WPRODVER_STR);

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowContextMenu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);

    // Update the checkmark for the startup option based on whether the startup key is set
    CheckMenuItem(hMenu, IDM_TOGGLE_STARTUP, MF_BYCOMMAND | (IsStartupKeySet() ? MF_CHECKED : MF_UNCHECKED));

    // Use TrackPopupMenuEx instead of TrackPopupMenu
    TrackPopupMenuEx(hMenu, TPM_RIGHTBUTTON | TPM_RECURSE | TPM_VERNEGANIMATION, pt.x, pt.y, hwnd, NULL);
}

void CreateContextMenu()
{
    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, IDM_VERSIONCHECK, L"Check for updates");
    AppendMenu(hMenu, MF_STRING, IDM_BUYMEACOFFEE, L"Buy Me a Coffee");
    AppendMenu(hMenu, MF_STRING, IDM_TOGGLE_STARTUP, L"Start with Windows");
    AppendMenu(hMenu, MF_STRING, IDM_EXITAPP, L"Exit");
}

void TaskBarCheckClick(LPARAM lParam, HWND hwnd)
{
    if (LOWORD(lParam) == WM_RBUTTONUP)
        ShowContextMenu(hwnd);

    else if (LOWORD(lParam) == WM_LBUTTONUP)
        IconClicked(hwnd);
}

void TaskBarCheckCommand(WORD cmd)
{
    switch (cmd)
    {
        case IDM_EXITAPP:
            PostQuitMessage(0);
            break;

        case IDM_BUYMEACOFFEE:
            ShellExecute(NULL, L"open", L"https://buymeacoffee.com/danieldownes/mouse-wrap-4", NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDM_VERSIONCHECK:
            ShellExecute(NULL, L"open", L"https://danieldownes.github.io/MouseWrap/check/?version=" WPRODVER_STR, NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDM_TOGGLE_STARTUP:
            if (IsStartupKeySet())
                RemoveFromStartup();
            else
                AddToStartup();

            break;
    }
}

void IconClicked(HWND hwnd)
{
    ToggleWrapEnabled(hwnd);

    BOOL isDarkTheme = IsDarkTheme();

    // Swap tray icon
    nid.hIcon = LoadIcon(hInstMain, MAKEINTRESOURCE((wrapEnabled ?
        (isDarkTheme ? IDI_ICON_ENABLED_DARK : IDI_ICON_ENABLED_LIGHT) :
        (isDarkTheme ? IDI_ICON_DISABLED_DARK : IDI_ICON_DISABLED_LIGHT))));
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void CleanUpTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
