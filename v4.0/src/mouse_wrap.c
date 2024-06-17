#include <windows.h>
#include "mouse_wrap.h"
#include <stdio.h>

BOOL wrapEnabled = TRUE;

typedef struct {
    POINT pt;
    BOOL isDragging;
} MouseData;

void ToggleWrapEnabled(HWND hwnd)
{
    wrapEnabled = !wrapEnabled;
    if (wrapEnabled)
        SetTimer(hwnd, IDT_WRAP_TIMER, WRAP_DELAY, NULL);
    else
        KillTimer(hwnd, IDT_WRAP_TIMER);
}

void WrapMouseWhileDragging()
{
    MouseData mouseData;
    GetCursorPos(&mouseData.pt);
    mouseData.isDragging = TRUE;

    // Enumerate all monitors and check if we need to wrap the mouse
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&mouseData);
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MouseData* mouseData = (MouseData*)dwData;
    RECT workArea;
    MONITORINFO mi;

    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &mi);
    workArea = mi.rcWork;

    // Check if the mouse is exactly on the border of the screen
    if (mouseData->pt.x <= lprcMonitor->left) {
        mouseData->pt.x = lprcMonitor->right - 2;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.x >= lprcMonitor->right - 1) {
        mouseData->pt.x = lprcMonitor->left + 2;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y <= lprcMonitor->top) {
        mouseData->pt.y = lprcMonitor->bottom - 2;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y == lprcMonitor->bottom - 1) {
        mouseData->pt.y = lprcMonitor->top + 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }

    // Check workspace area only while dragging
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        return;

    // Check if the mouse is exactly on the border and wrap it to the opposite side within the work area
    if (mouseData->pt.x == workArea.left) {
        mouseData->pt.x = workArea.right - 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.x == workArea.right) {
        mouseData->pt.x = workArea.left;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y == workArea.top) {
        mouseData->pt.y = workArea.bottom - 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y == workArea.bottom - 1) {
        mouseData->pt.y = workArea.top;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }

    // Continue enumeration
    return TRUE;
}

