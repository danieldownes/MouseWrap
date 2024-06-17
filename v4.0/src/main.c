#include <windows.h>
#include "resource.h"
#include "mouse_wrap.h"
#include "taskbar.h"

#define CLASS_NAME L"Sample Window Class"
#define WINDOW_TITLE L"Sample Window"
#define IDT_TIMER1 1

HINSTANCE hInst;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    CreateTrayIcon(hwnd, hInstance);
    CreateContextMenu();

    SetTimer(hwnd, IDT_TIMER1, 100, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanUpTrayIcon();

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_USER + 1: // WM_TRAYICON
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
