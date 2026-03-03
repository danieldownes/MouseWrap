#ifndef MOUSE_WRAP_H
#define MOUSE_WRAP_H

#include "multimonitor_edges.h"
#include "multimonitor_contour.h"

#define IDT_WRAP_TIMER 1
// Polling interval in milliseconds for the mouse-wrap timer.
// 20 ms (~50 Hz) balances responsiveness with low CPU usage.
#define WRAP_DELAY 20

extern BOOL wrapEnabled;

extern me_Rect* g_monitor_rects_desktop;
extern SIZE_T   g_monitor_count_desktop;

extern EdgeList* g_desktop_contour;
extern EdgeList* g_disabled_edges;

void ToggleWrapEnabled(HWND hwnd);
void WrapMouseWhileDragging();

// Functions for managing monitor contours
void UpdateMonitorContours(void);
void CleanupGlobalContourResources(void);

// Test whether a point is within tolerance pixels of an edge segment
BOOL IsPointNearEdge(POINT pt, me_Edge edge, int tolerance);

// Disabled edge management
BOOL IsEdgeDisabled(me_Edge edge);
void ToggleEdgeDisabled(me_Edge edge);
void SaveDisabledEdges(void);
void LoadDisabledEdges(void);

#endif // MOUSE_WRAP_H