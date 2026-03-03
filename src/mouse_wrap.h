#ifndef MOUSE_WRAP_H
#define MOUSE_WRAP_H

#include "multimonitor_edges.h"
#include "multimonitor_contour.h"

#define IDT_WRAP_TIMER 1
// Polling interval in milliseconds for the mouse-wrap timer.
// 20 ms (~50 Hz) balances responsiveness with low CPU usage.
#define WRAP_DELAY 20

// Three-state edge wrapping
typedef enum {
    EDGE_WRAP    = 0,  // Green  — immediate wrap (default)
    EDGE_DELAYED = 1,  // Yellow — 100 ms delay then wrap
    EDGE_NOWRAP  = 2   // Red    — never wrap
} EdgeState;

extern BOOL wrapEnabled;

extern me_Rect* g_monitor_rects_desktop;
extern SIZE_T   g_monitor_count_desktop;

extern EdgeList* g_desktop_contour;
extern EdgeList* g_disabled_edges;
extern EdgeList* g_delayed_edges;

void ToggleWrapEnabled(HWND hwnd);
void WrapMouseWhileDragging();

// Functions for managing monitor contours
void UpdateMonitorContours(void);
void CleanupGlobalContourResources(void);

// Test whether a point is within tolerance pixels of an edge segment
BOOL IsPointNearEdge(POINT pt, me_Edge edge, int tolerance);

// Edge state management
EdgeState GetEdgeState(me_Edge edge);
void CycleEdgeState(me_Edge edge);
void SaveEdgeStates(void);
void LoadEdgeStates(void);

// Configurable delay for EDGE_DELAYED edges (milliseconds)
extern DWORD g_edge_delay_ms;
void SaveDelayMs(void);
void LoadDelayMs(void);

#endif // MOUSE_WRAP_H