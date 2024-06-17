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
    wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t), L"Mouse Wrap");

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
    AppendMenu(hMenu, MF_STRING, 1002, L"Buy Me a Coffee");
    AppendMenu(hMenu, MF_STRING, 1001, L"Exit");
}

void TaskBarCheckClick(LPARAM lParam, HWND hwnd)
{
    if (LOWORD(lParam) == WM_RBUTTONUP)
    {
        ShowContextMenu(hwnd);
    }
    else if (LOWORD(lParam) == WM_LBUTTONUP)
    {
        MessageBox(hwnd, L"Tray icon clicked!", L"Info", MB_OK | MB_ICONINFORMATION);
    }
}

void TaskBarCheckCommand(WORD cmd)
{
    if (cmd == 1001)
        PostQuitMessage(0);

    if (cmd == 1002)
        ShellExecute(NULL, L"open", L"https://buymeacoffee.com/danieldownes/mouse-wrap-4", NULL, NULL, SW_SHOWNORMAL);
}

void CleanUpTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
