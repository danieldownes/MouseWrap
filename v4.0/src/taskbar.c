#include "taskbar.h"

#define WM_TRAYICON (WM_USER + 1)

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
    wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t), L"Tray Icon Example");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowContextMenu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);

    // Use TrackPopupMenuEx instead of TrackPopupMenu
    TrackPopupMenuEx(hMenu, TPM_RIGHTBUTTON | TPM_RECURSE | TPM_VERNEGANIMATION | TPM_LAYOUTRTL, pt.x, pt.y, hwnd, NULL);
}

void CreateContextMenu()
{
    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1001, L"Exit");
}

void CleanUpTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
