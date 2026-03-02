#include "main.h"
#include "mouse_wrap.h" // For UpdateMonitorContours and CleanupGlobalContourResources
#include <windows.h>
#include <ShellScalingApi.h> // For SetProcessDpiAwarenessContext

#pragma comment(lib, "Shcore.lib") // Link against Shcore.lib for DPI awareness functions

HINSTANCE hInst;
HANDLE hMutexSingleInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // Set DPI awareness to Per-Monitor DPI Aware V2
    // This should be one of the first calls in the application
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	OutputDebugStringA("MouseWrap: WinMain entered.\n");

    if (CheckAlreadyRunning())
        return 0;

	OutputDebugStringA("MouseWrap: Mutex created.\n");

    hInst = hInstance;

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON_ENABLED_DARK));
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
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

	OutputDebugStringA("MouseWrap: Window created.\n");

    if (hwnd == NULL)
        return 0;

    

    CreateTrayIcon(hwnd, hInstance);
	OutputDebugStringA("MouseWrap: Tray icon created.\n");

    CreateContextMenu();
    wrapEnabled = FALSE;
    ToggleWrapEnabled(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CleanUpTrayIcon();
    CleanupGlobalContourResources(); // Clean up contour data
    ReleaseMutex(hMutexSingleInstance);
    CloseHandle(hMutexSingleInstance);

    return (int)msg.wParam;
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
    case WM_DISPLAYCHANGE: // Monitor configuration changed
        UpdateMonitorContours(); // Renamed function
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        if (wParam == IDT_WRAP_TIMER)
            WrapMouseWhileDragging();
        break;
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL CheckAlreadyRunning()
{
    hMutexSingleInstance = CreateMutexW(NULL, FALSE, L"MouseWrapSingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxW(NULL, L"Mouse Wrap is already running.", L"Error", MB_OK | MB_ICONERROR);
        return TRUE;
    }
    return FALSE;
}