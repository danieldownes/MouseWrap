#include "darkmode.h"
#include <uxtheme.h>
#include <dwmapi.h>
#include <string.h>

// --- Undocumented uxtheme.dll entry points (Windows 10 1809+) ---
// Ordinal 135: SetPreferredAppMode(PreferredAppMode)  (1809: AllowDarkModeForApp)
// Ordinal 133: AllowDarkModeForWindow(HWND, BOOL)
// Ordinal 104: RefreshImmersiveColorPolicyState(void)
// They are optional: every use below is NULL-guarded.

typedef BOOL (WINAPI *AllowDarkModeForWindowFunc)(HWND, BOOL);
typedef BOOL (WINAPI *SetPreferredAppModeFunc)(int);
typedef void (WINAPI *RefreshImmersiveColorPolicyStateFunc)(void);

static AllowDarkModeForWindowFunc          pAllowDarkModeForWindow = NULL;
static SetPreferredAppModeFunc             pSetPreferredAppMode = NULL;
static RefreshImmersiveColorPolicyStateFunc pRefreshImmersiveColorPolicyState = NULL;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// GetProcAddress returns FARPROC; copy the pointer bits into the typed
// function pointer rather than casting, which keeps -Wpedantic and
// -Wcast-function-type quiet (all function pointers share one size on Windows).
#define LOAD_ORDINAL(dst, hMod, ord) do { \
        FARPROC p_ = GetProcAddress((hMod), MAKEINTRESOURCEA(ord)); \
        memcpy(&(dst), &p_, sizeof(dst)); \
    } while (0)

void DarkMode_Init(void)
{
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme) return;

    LOAD_ORDINAL(pSetPreferredAppMode,              hUxTheme, 135);
    LOAD_ORDINAL(pAllowDarkModeForWindow,           hUxTheme, 133);
    LOAD_ORDINAL(pRefreshImmersiveColorPolicyState, hUxTheme, 104);

    if (pSetPreferredAppMode)
        pSetPreferredAppMode(1); // AllowDark: follow the system setting
    if (pRefreshImmersiveColorPolicyState)
        pRefreshImmersiveColorPolicyState();
}

BOOL DarkMode_IsEnabled(void)
{
    HKEY hKey;
    DWORD dwValue = 1;
    DWORD dwSize = sizeof(DWORD);
    BOOL dark = FALSE;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            dark = (dwValue == 0); // 0 = dark
        RegCloseKey(hKey);
    }
    return dark;
}

void DarkMode_ApplyToWindow(HWND hwnd, BOOL dark)
{
    // Documented: dark title bar / frame (Windows 10 20H1+, Windows 11)
    BOOL value = dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

    if (pAllowDarkModeForWindow)
        pAllowDarkModeForWindow(hwnd, dark);
    if (pRefreshImmersiveColorPolicyState)
        pRefreshImmersiveColorPolicyState();
}

void DarkMode_ApplyToControl(HWND hCtl, BOOL dark)
{
    WCHAR cls[64] = L"";
    GetClassNameW(hCtl, cls, 64);

    if (pAllowDarkModeForWindow)
        pAllowDarkModeForWindow(hCtl, dark);

    // The "DarkMode_*" theme classes are what Explorer itself uses; the light
    // equivalents restore the default themed look.
    if (lstrcmpiW(cls, L"ComboBox") == 0) {
        SetWindowTheme(hCtl, dark ? L"DarkMode_CFD" : L"CFD", NULL);
    } else if (lstrcmpiW(cls, L"Button") == 0
            || lstrcmpiW(cls, L"ScrollBar") == 0
            || lstrcmpiW(cls, L"tooltips_class32") == 0) {
        SetWindowTheme(hCtl, dark ? L"DarkMode_Explorer" : L"Explorer", NULL);
    }
    // Trackbar and static controls are drawn by the dialog itself.
}
