#include "main.h"

HINSTANCE hInst;
HANDLE hMutexSingleInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    if (CheckAlreadyRunning())
		return 0;

    hInst = hInstance;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_ENABLED_DARK));

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
        return 0;

    CreateTrayIcon(hwnd, hInstance);
    CreateContextMenu();

    wrapEnabled = FALSE;
    ToggleWrapEnabled(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanUpTrayIcon();

    // Release the mutex
    ReleaseMutex(hMutexSingleInstance);
    CloseHandle(hMutexSingleInstance);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_USER + 1: // WM_TRAYICON
            TaskBarCheckClick(lParam, hwnd);
            break;

        case WM_COMMAND:
            TaskBarCheckCommand(LOWORD(wParam));
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_TIMER:
            if (wParam == IDT_WRAP_TIMER)
                WrapMouseWhileDragging();

            break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL CheckAlreadyRunning()
{
    hMutexSingleInstance = CreateMutex(NULL, FALSE, L"MouseWrapSingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(NULL, L"Mouse Wrap is already running.", L"Error", MB_OK | MB_ICONERROR);
        return TRUE;
    }
    return FALSE;
}