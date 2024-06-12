#include <windows.h>
#include <shellapi.h>
#include "resource.h"
#include "mouse_wrap.h"

#define CLASS_NAME L"Sample Window Class"
#define WINDOW_TITLE L"Sample Window"
#define WM_TRAYICON (WM_USER + 1)
#define IDT_TIMER1 1

NOTIFYICONDATA nid;
HINSTANCE hInst;
HMENU hMenu;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void CreateTrayIcon(HWND hwnd)
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        0, // No window
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    CreateTrayIcon(hwnd);
    CreateContextMenu();

    SetTimer(hwnd, IDT_TIMER1, 100, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_TRAYICON:
        if (LOWORD(lParam) == WM_RBUTTONUP)
        {
            ShowContextMenu(hwnd);
        }
        else if (LOWORD(lParam) == WM_LBUTTONUP)
        {
            MessageBox(hwnd, L"Tray icon clicked!", L"Info", MB_OK | MB_ICONINFORMATION);
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1001)
        {
            PostQuitMessage(0);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_TIMER:
        if (wParam == IDT_TIMER1)
        {
            WrapMouseWhileDragging();
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
