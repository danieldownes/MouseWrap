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

// Test whether a point is on an edge segment: on pixel c-1 or c for an edge
// at coordinate c, widened by tolerance extra pixels on each side.
BOOL IsPointNearEdge(POINT pt, me_Edge edge, int tolerance);

// Remove edges from list that do not lie on any edge of reference.
// Used to strip taskbar-boundary edges from the workspace contour.
void RemoveEdgesNotOnContour(EdgeList* list, const EdgeList* reference);

// Edge state management.  Edge states are stored per monitor layout
// (see GetLayoutId); Save/LoadEdgeStates act on the current layout.
EdgeState GetEdgeState(me_Edge edge);
void CycleEdgeState(me_Edge edge);
void SaveEdgeStates(void);
void LoadEdgeStates(void);

// Identify a monitor layout: the same set of desktop rectangles, in any
// order, always yields the same id.  Writes a NUL-terminated 16-char hex
// string to out (cch must be >= LAYOUT_ID_CCH).
#define LAYOUT_ID_CCH 17
void GetLayoutId(const me_Rect* rects, SIZE_T count, WCHAR* out, size_t cch);

// Configurable delay for EDGE_DELAYED edges (milliseconds)
extern DWORD g_edge_delay_ms;
void SaveDelayMs(void);
void LoadDelayMs(void);

// How wrapping behaves while the primary mouse button is held (i.e. while
// dragging a window or selection).  This takes precedence over each edge's
// own Wrap / Delayed setting; edges set to No Wrap never wrap regardless.
// Values match the order of the "While dragging" dropdown in Options.
typedef enum {
    DRAG_WRAP_DELAYED = 0,  // every wrapping edge acts as Delayed while dragging (default)
    DRAG_WRAP_INSTANT = 1,  // every wrapping edge wraps instantly while dragging
    DRAG_WRAP_NONE    = 2   // nothing wraps while dragging
} DragWrapMode;

extern DragWrapMode g_drag_wrap_mode;
void SaveDragWrapMode(void);
void LoadDragWrapMode(void);

// TRUE while the primary mouse button (honouring SM_SWAPBUTTON) is held down.
BOOL IsPrimaryButtonDown(void);

// Apply g_drag_wrap_mode to an edge's configured state while dragging.
// Wrap and Delayed edges follow the drag mode; No Wrap edges stay No Wrap.
EdgeState ResolveEdgeState(EdgeState configured, BOOL dragging);

#endif // MOUSE_WRAP_H