#include "taskbar.h"
#include "startup.h"

#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXITAPP 1001
#define IDM_BUYMEACOFFEE 1002
#define IDM_TOGGLE_STARTUP 1003

static NOTIFYICONDATA nid;
static HMENU hMenu;

void CreateTrayIcon(HWND hwnd, HINSTANCE hInst)
{
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
    wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t), L"Mouse Wrap");

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
    AppendMenu(hMenu, MF_STRING, IDM_BUYMEACOFFEE, L"Buy Me a Coffee");
    AppendMenu(hMenu, MF_STRING, IDM_TOGGLE_STARTUP, L"Start with Windows");
    AppendMenu(hMenu, MF_STRING, IDM_EXITAPP, L"Exit");
}

void TaskBarCheckClick(LPARAM lParam, HWND hwnd)
{
    if (LOWORD(lParam) == WM_RBUTTONUP)
        ShowContextMenu(hwnd);

    else if (LOWORD(lParam) == WM_LBUTTONUP)
        MessageBox(hwnd, L"Tray icon clicked!", L"Info", MB_OK | MB_ICONINFORMATION);
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

        case IDM_TOGGLE_STARTUP:
            if (IsStartupKeySet())
                RemoveFromStartup();
            else
                AddToStartup();

            break;
    }
}

void CleanUpTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
