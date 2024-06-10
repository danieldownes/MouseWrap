#include <windows.h>
#include <shellapi.h>
#include "main.h"
#include "mouse_wrap.h"

NOTIFYICONDATA nid;
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Default icon for the window

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_HIDE);

    // Initialize NOTIFYICONDATA structure
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1001;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Default application icon
    strcpy_s(nid.szTip, sizeof(nid.szTip), "Tray Icon");

    Shell_NotifyIcon(NIM_ADD, &nid);

    // Set a timer to check and wrap the mouse position every 10 ms
    SetTimer(hwnd, IDT_TIMER1, 10, (TIMERPROC)NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        return 0;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN)
        {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1, "Exit");
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            DestroyWindow(hwnd);
        }
        break;

    case WM_TIMER:
        if (wParam == IDT_TIMER1)
        {
            // WrapMouse();
            WrapMouseWhileDragging();
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
