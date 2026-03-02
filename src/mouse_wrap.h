#ifndef MOUSE_WRAP_H
#define MOUSE_WRAP_H

#define IDT_WRAP_TIMER 1
#define WRAP_DELAY 20

BOOL wrapEnabled;

void ToggleWrapEnabled(HWND hwnd);
void WrapMouseWhileDragging();
// BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData); // Old, no longer used by main.c

// Functions for managing the desktop contour
void UpdateDesktopContour(void); // Renamed back
void CleanupGlobalContourResources(void);

#endif // MOUSE_WRAP_H