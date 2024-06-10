#include <windows.h>
#include "mouse_wrap.h"

void WrapMouse()
{
    POINT pt;
    RECT screenRect;

    // Get the current mouse position
    GetCursorPos(&pt);

    // Get the screen dimensions
    screenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screenRect.right = screenRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screenRect.bottom = screenRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Check if the mouse is out of bounds and wrap it to the opposite side
    if (pt.x <= screenRect.left) {
        pt.x = screenRect.right - 1;
    }
    else if (pt.x >= screenRect.right) {
        pt.x = screenRect.left + 1;
    }
    else if (pt.y <= screenRect.top) {
        pt.y = screenRect.bottom - 1;
    }
    else if (pt.y >= screenRect.bottom) {
        pt.y = screenRect.top + 1;
    }

    // Set the new mouse position
    SetCursorPos(pt.x, pt.y);
}
