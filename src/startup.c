#include "startup.h"
#include <strsafe.h> // For StringCchPrintf

#define STARTUP_KEY_PATH L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define STARTUP_VALUE_NAME L"MouseWrap"

// Helper function to open a registry key
HKEY OpenStartupKey(DWORD dwDesiredAccess)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, STARTUP_KEY_PATH, 0, dwDesiredAccess, &hKey) != ERROR_SUCCESS)
        return NULL;

    return hKey;
}

// Helper function to check if a file exists
BOOL FileExists(LPCWSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void AddToStartup()
{
    HKEY hKey = OpenStartupKey(KEY_SET_VALUE);
    if (hKey == NULL)
        return;

    // Get the full path of the executable
    wchar_t szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    // Set the value for the startup key
    RegSetValueEx(hKey, STARTUP_VALUE_NAME, 0, REG_SZ, (BYTE*)szPath, (lstrlen(szPath) + 1) * sizeof(wchar_t));
    RegCloseKey(hKey);
}

void RemoveFromStartup()
{
    HKEY hKey = OpenStartupKey(KEY_SET_VALUE);
    if (hKey == NULL)
        return;

    // Delete the startup key value
    RegDeleteValue(hKey, STARTUP_VALUE_NAME);
    RegCloseKey(hKey);
}

BOOL IsStartupKeySet()
{
    HKEY hKey = OpenStartupKey(KEY_QUERY_VALUE);
    if (hKey == NULL)
        return FALSE;

    DWORD dwSize = 0;
    LONG lResult = RegQueryValueEx(hKey, STARTUP_VALUE_NAME, NULL, NULL, NULL, &dwSize);
    RegCloseKey(hKey);

    return (lResult == ERROR_SUCCESS && dwSize > 0);
}

void UpdateStartupPathIfMoved()
{
    wchar_t szPath[MAX_PATH];
    wchar_t szCurrentPath[MAX_PATH];
    DWORD dwSize = sizeof(szPath);
    HKEY hKey = OpenStartupKey(KEY_QUERY_VALUE | KEY_SET_VALUE);
    
    if (hKey == NULL)
        return;

    // Get the full path of the current executable
    GetModuleFileName(NULL, szCurrentPath, MAX_PATH);

    // Query the current path set in the registry
    if (RegQueryValueEx(hKey, STARTUP_VALUE_NAME, NULL, NULL, (BYTE*)szPath, &dwSize) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return;
    }

    // Check if the current path matches the path in the registry
    if (!FileExists(szPath) || lstrcmpi(szPath, szCurrentPath) != 0)
    {
        // Update the path in the registry if it has changed
        RegSetValueEx(hKey, STARTUP_VALUE_NAME, 0, REG_SZ, (BYTE*)szCurrentPath, (lstrlen(szCurrentPath) + 1) * sizeof(wchar_t));
    }
    RegCloseKey(hKey);
}
