#include <windows.h>
#include "mouse_wrap.h"
#include <stdio.h>

typedef struct {
    POINT pt;
    BOOL isDragging;
} MouseData;

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MouseData* mouseData = (MouseData*)dwData;
    RECT workArea;
    MONITORINFO mi;

    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &mi);
    workArea = mi.rcWork;

    // Use OutputDebugString to output the WorkArea dimensions
    char buffer[256];
    sprintf_s(buffer, sizeof(buffer), "WorkArea: (%d, %d) - (%d, %d)\n", workArea.left, workArea.top, workArea.right, workArea.bottom);
    OutputDebugStringA(buffer);
    sprintf_s(buffer, sizeof(buffer), "Monitor: (%d, %d) - (%d, %d)\n", lprcMonitor->right, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom);
    OutputDebugStringA(buffer);


    // Check if the mouse is exactly on the border of the screen
    if (mouseData->pt.x <= lprcMonitor->left) {
        mouseData->pt.x = lprcMonitor->right - 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.x >= lprcMonitor->right - 1) {
        mouseData->pt.x = lprcMonitor->left + 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y <= lprcMonitor->top) {
        mouseData->pt.y = lprcMonitor->bottom - 1;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }
    else if (mouseData->pt.y == lprcMonitor->bottom - 1) {
        mouseData->pt.y = lprcMonitor->top;
        SetCursorPos(mouseData->pt.x, mouseData->pt.y);
        return FALSE;
    }


    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        return;
    }

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


    return TRUE; // Continue enumeration
}

void WrapMouseWhileDragging()
{
    MouseData mouseData;
    GetCursorPos(&mouseData.pt);
    mouseData.isDragging = TRUE;

    // Enumerate all monitors and check if we need to wrap the mouse
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&mouseData);
}
