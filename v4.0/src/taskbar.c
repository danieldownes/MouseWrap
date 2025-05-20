#include "taskbar.h"
#include <string.h> // For standard memset

#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXITAPP 1001
#define IDM_BUYMEACOFFEE 1002
#define IDM_TOGGLE_STARTUP 1003
#define IDM_VERSIONCHECK 1004

HINSTANCE hInstMain;
static NOTIFYICONDATA nid;
static HMENU hMenu;

// Custom memset implementation
void* m_memset(void* dest, int c, size_t count) {
    unsigned char* p = (unsigned char*)dest;
    while (count--) {
        *p++ = (unsigned char)c;
    }
    return dest;
}

BOOL IsDarkTheme(void)
{
    HKEY hKey;
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(DWORD);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
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
    OutputDebugStringW(L"MouseWrap: CreateTrayIcon entered.\n");

    memset(&nid, 0, sizeof(nid));
    OutputDebugStringW(L"MouseWrap: nid memset complete.\n");

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    OutputDebugStringW(L"MouseWrap: nid struct basic members initialized.\n");

    BOOL isDarkTheme = IsDarkTheme();
    WCHAR themeMsg[100];
    wsprintfW(themeMsg, L"MouseWrap: IsDarkTheme() returned: %s.\n", isDarkTheme ? L"TRUE" : L"FALSE");
    OutputDebugStringW(themeMsg);

    UINT iconID = isDarkTheme ? IDI_ICON_ENABLED_DARK : IDI_ICON_ENABLED_LIGHT;
    WCHAR loadMsg[128];
    wsprintfW(loadMsg, L"MouseWrap: Attempting to load themed icon ID: %u with hInstMain (0x%p).\n", iconID, (void*)hInstMain);
    OutputDebugStringW(loadMsg);
    
    nid.hIcon = LoadIconW(hInstMain, MAKEINTRESOURCEW(iconID));

    if (nid.hIcon == NULL) {
        WCHAR szErrorMsg[256];
        wsprintfW(szErrorMsg, L"MouseWrap: Failed to load themed icon resource. Error: %lu. Resource ID: %u\n",
                  GetLastError(), iconID);
        OutputDebugStringW(szErrorMsg);
        
        OutputDebugStringW(L"MouseWrap: Themed icon failed, attempting to load IDI_APPLICATION as fallback.\n");
        nid.hIcon = LoadIconW(NULL, IDI_APPLICATION); // Fallback to system icon
        if (nid.hIcon == NULL) {
            wsprintfW(szErrorMsg, L"MouseWrap: Failed to load fallback system icon IDI_APPLICATION. Error: %lu\n", GetLastError());
            OutputDebugStringW(szErrorMsg);
        } else {
             OutputDebugStringW(L"MouseWrap: Fallback IDI_APPLICATION loaded successfully.\n");
        }
    } else {
        OutputDebugStringW(L"MouseWrap: Themed icon loaded successfully.\n");
    }

    lstrcpynW(nid.szTip, WINDOW_TITLE L" " WPRODVER_STR, ARRAYSIZE(nid.szTip));
    OutputDebugStringW(L"MouseWrap: After lstrcpynW for szTip.\n");
    
    if (nid.hIcon != NULL) {
        OutputDebugStringW(L"MouseWrap: nid.hIcon is valid, attempting Shell_NotifyIconW NIM_ADD.\n");
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            WCHAR szErrorMsg[256];
            wsprintfW(szErrorMsg, L"MouseWrap: Shell_NotifyIconW NIM_ADD failed. Error: %lu\n", GetLastError());
            OutputDebugStringW(szErrorMsg);
        } else {
            OutputDebugStringW(L"MouseWrap: Shell_NotifyIconW NIM_ADD called successfully.\n");
        }
    } else {
        OutputDebugStringW(L"MouseWrap: nid.hIcon is NULL, Shell_NotifyIconW NIM_ADD was not called (initial load and fallback failed).\n");
    }
}

void ShowContextMenu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    CheckMenuItem(hMenu, IDM_TOGGLE_STARTUP, MF_BYCOMMAND | (IsStartupKeySet() ? MF_CHECKED : MF_UNCHECKED));
    TrackPopupMenuEx(hMenu, TPM_RIGHTBUTTON | TPM_RECURSE | TPM_VERNEGANIMATION, pt.x, pt.y, hwnd, NULL);
}

void CreateContextMenu(void)
{
    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, IDM_VERSIONCHECK, L"Check for updates");
    AppendMenuW(hMenu, MF_STRING, IDM_BUYMEACOFFEE, L"Buy Me a Coffee");
    AppendMenuW(hMenu, MF_STRING, IDM_TOGGLE_STARTUP, L"Start with Windows");
    AppendMenuW(hMenu, MF_STRING, IDM_EXITAPP, L"Exit");
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
        ShellExecuteW(NULL, L"open", L"https://buymeacoffee.com/danieldownes/mouse-wrap-4", NULL, NULL, SW_SHOWNORMAL);
        break;
    case IDM_VERSIONCHECK:
        ShellExecuteW(NULL, L"open", L"https://danieldownes.github.io/MouseWrap/check/?version=" WPRODVER_STR, NULL, NULL, SW_SHOWNORMAL);
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
    HICON hNewIcon = LoadIconW(hInstMain, MAKEINTRESOURCEW(wrapEnabled ?
        (isDarkTheme ? IDI_ICON_ENABLED_DARK : IDI_ICON_ENABLED_LIGHT) :
        (isDarkTheme ? IDI_ICON_DISABLED_DARK : IDI_ICON_DISABLED_LIGHT)));

    if (hNewIcon == NULL) {
        WCHAR szErrorMsg[256];
        wsprintfW(szErrorMsg, L"MouseWrap: Failed to load icon in IconClicked. Error: %lu. Resource ID: %d\n",
            GetLastError(),
            wrapEnabled ? (isDarkTheme ? IDI_ICON_ENABLED_DARK : IDI_ICON_ENABLED_LIGHT) : (isDarkTheme ? IDI_ICON_DISABLED_DARK : IDI_ICON_DISABLED_LIGHT)
        );
        OutputDebugStringW(szErrorMsg);
        // Keep the old icon if loading the new one fails
    } else {
        // Destroy the old icon if it's different and valid before assigning the new one
        // This check is important if LoadIcon creates a new copy each time.
        // However, for shared resources loaded via MAKEINTRESOURCE, direct DestroyIcon might not be needed
        // unless you explicitly copied it. For now, direct assignment is typical.
        // if (nid.hIcon && nid.hIcon != hNewIcon) {
        //     DestroyIcon(nid.hIcon); // Be cautious with DestroyIcon on shared resources
        // }
        nid.hIcon = hNewIcon;
    }

    // Only attempt to modify if nid.hIcon is valid.
    // If initial loading failed and fallback also failed, nid.hIcon could be NULL.
    if (nid.hIcon == NULL) {
        OutputDebugStringW(L"MouseWrap: nid.hIcon is NULL in IconClicked, cannot modify tray icon.\n");
        return;
    }

    if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        WCHAR szErrorMsg[256];
        wsprintfW(szErrorMsg, L"MouseWrap: Shell_NotifyIconW NIM_MODIFY failed in IconClicked. Error: %lu\n", GetLastError());
        OutputDebugStringW(szErrorMsg);
    }
}

void CleanUpTrayIcon(void)
{
    Shell_NotifyIconW(NIM_DELETE, &nid);
}
