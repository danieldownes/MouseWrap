#ifndef MOUSE_WRAP_H
#define MOUSE_WRAP_H

#include "multimonitor_edges.h"

#define IDT_WRAP_TIMER 1
// Polling interval in milliseconds for the mouse-wrap timer.
// 20 ms (~50 Hz) balances responsiveness with low CPU usage.
#define WRAP_DELAY 20

extern BOOL wrapEnabled;

void ToggleWrapEnabled(HWND hwnd);
void WrapMouseWhileDragging();

// Functions for managing monitor contours
void UpdateMonitorContours(void);
void CleanupGlobalContourResources(void);

// Test whether a point is within tolerance pixels of an edge segment
BOOL IsPointNearEdge(POINT pt, me_Edge edge, int tolerance);

#endif // MOUSE_WRAP_H