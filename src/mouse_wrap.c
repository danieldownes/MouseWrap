#include <windows.h>
#include "mouse_wrap.h"
#include "multimonitor_edges.h"
#include "multimonitor_contour.h"
#include <stdio.h> // For temporary debugging

// Global variable to store the calculated desktop contour
EdgeList* g_desktop_contour = NULL;
// Global variable to store the calculated workspace contour
EdgeList* g_workspace_contour = NULL;

// Global variable to store monitor rectangles (full area)
me_Rect* g_monitor_rects_desktop = NULL;
SIZE_T g_monitor_count_desktop = 0;

// Global variable to store monitor work area rectangles
me_Rect* g_monitor_rects_workspace = NULL;
SIZE_T g_monitor_count_workspace = 0;

BOOL wrapEnabled = TRUE;

typedef struct {
    POINT pt;
    BOOL isDragging; 
} MouseData;

// Structure to help collect monitor RECTs
typedef struct {
    me_Rect* rects;
    SIZE_T count;
    SIZE_T capacity;
    BOOL use_work_area; 
} MonitorRectCollector;

BOOL CALLBACK CollectMonitorRectsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MonitorRectCollector* collector = (MonitorRectCollector*)dwData;
    if (collector->count >= collector->capacity) {
        return FALSE; 
    }

    RECT area_to_use;
    if (collector->use_work_area) {
        MONITORINFO mi;
        mi.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(hMonitor, &mi)) {
            area_to_use = mi.rcWork;
        } else {
            area_to_use = *lprcMonitor; 
        }
    } else {
        area_to_use = *lprcMonitor;
    }

    collector->rects[collector->count].xMin = area_to_use.left;
    collector->rects[collector->count].xMax = area_to_use.right;
    collector->rects[collector->count].yMin = area_to_use.top;
    collector->rects[collector->count].yMax = area_to_use.bottom;
    collector->count++;
    return TRUE; 
}

void FreeGlobalMonitorRectArrays() {
    if (g_monitor_rects_desktop != NULL) {
        HeapFree(GetProcessHeap(), 0, g_monitor_rects_desktop);
        g_monitor_rects_desktop = NULL;
        g_monitor_count_desktop = 0;
    }
    if (g_monitor_rects_workspace != NULL) {
        HeapFree(GetProcessHeap(), 0, g_monitor_rects_workspace);
        g_monitor_rects_workspace = NULL;
        g_monitor_count_workspace = 0;
    }
}

void UpdateMonitorContours() { // Renamed from UpdateDesktopContour
    if (g_desktop_contour != NULL) {
        edge_list_free(g_desktop_contour);
        g_desktop_contour = NULL;
    }
    if (g_workspace_contour != NULL) {
        edge_list_free(g_workspace_contour);
        g_workspace_contour = NULL;
    }
    FreeGlobalMonitorRectArrays();

    int raw_monitor_count = GetSystemMetrics(SM_CMONITORS);
    if (raw_monitor_count <= 0) raw_monitor_count = 1;

    g_monitor_rects_desktop = (me_Rect*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, raw_monitor_count * sizeof(me_Rect));
    if (g_monitor_rects_desktop == NULL) return; 

    MonitorRectCollector desktop_collector;
    desktop_collector.rects = g_monitor_rects_desktop;
    desktop_collector.count = 0;
    desktop_collector.capacity = raw_monitor_count;
    desktop_collector.use_work_area = FALSE; 
    EnumDisplayMonitors(NULL, NULL, CollectMonitorRectsProc, (LPARAM)&desktop_collector);
    g_monitor_count_desktop = desktop_collector.count;

    if (g_monitor_count_desktop > 0) {
        char dbg_buf[256];
        OutputDebugStringA("--- Calculating Desktop Contour ---\nMonitors (Desktop):\n");
        for (SIZE_T i = 0; i < g_monitor_count_desktop; ++i) {
            sprintf_s(dbg_buf, sizeof(dbg_buf), "  Monitor %zu: L:%ld T:%ld R:%ld B:%ld\n", i,
                    g_monitor_rects_desktop[i].xMin, g_monitor_rects_desktop[i].yMin,
                    g_monitor_rects_desktop[i].xMax, g_monitor_rects_desktop[i].yMax);
            OutputDebugStringA(dbg_buf);
        }
        g_desktop_contour = get_contour(g_monitor_rects_desktop, g_monitor_count_desktop);
        sprintf_s(dbg_buf, sizeof(dbg_buf), "Desktop Contour Edges: %zu\n", g_desktop_contour ? g_desktop_contour->size : 0);
        OutputDebugStringA(dbg_buf);
        if (g_desktop_contour) {
            for (SIZE_T i = 0; i < g_desktop_contour->size; ++i) {
                me_Edge e = g_desktop_contour->edges[i];
                sprintf_s(dbg_buf, sizeof(dbg_buf), "  DCEdge %zu: (%ld,%ld)-(%ld,%ld)\n", i, e.x1, e.y1, e.x2, e.y2);
                OutputDebugStringA(dbg_buf);
            }
        }
    } else {
        g_desktop_contour = create_edge_list(0);
        OutputDebugStringA("Desktop Contour: No monitors or 0 edges.\n");
    }

    // --- Workspace Contour ---
    g_monitor_rects_workspace = (me_Rect*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, raw_monitor_count * sizeof(me_Rect));
    if (g_monitor_rects_workspace == NULL) return;

    MonitorRectCollector workspace_collector;
    workspace_collector.rects = g_monitor_rects_workspace;
    workspace_collector.count = 0;
    workspace_collector.capacity = raw_monitor_count;
    workspace_collector.use_work_area = TRUE;
    EnumDisplayMonitors(NULL, NULL, CollectMonitorRectsProc, (LPARAM)&workspace_collector);
    g_monitor_count_workspace = workspace_collector.count;

    if (g_monitor_count_workspace > 0) {
        char dbg_buf[256];
        OutputDebugStringA("--- Calculating Workspace Contour ---\nMonitors (Workspace):\n");
        for (SIZE_T i = 0; i < g_monitor_count_workspace; ++i) {
            sprintf_s(dbg_buf, sizeof(dbg_buf), "  Monitor %zu: L:%ld T:%ld R:%ld B:%ld\n", i,
                    g_monitor_rects_workspace[i].xMin, g_monitor_rects_workspace[i].yMin,
                    g_monitor_rects_workspace[i].xMax, g_monitor_rects_workspace[i].yMax);
            OutputDebugStringA(dbg_buf);
        }
        g_workspace_contour = get_contour(g_monitor_rects_workspace, g_monitor_count_workspace);
        sprintf_s(dbg_buf, sizeof(dbg_buf), "Workspace Contour Edges: %zu\n", g_workspace_contour ? g_workspace_contour->size : 0);
        OutputDebugStringA(dbg_buf);
        if (g_workspace_contour) {
            for (SIZE_T i = 0; i < g_workspace_contour->size; ++i) {
                me_Edge e = g_workspace_contour->edges[i];
                sprintf_s(dbg_buf, sizeof(dbg_buf), "  WCEdge %zu: (%ld,%ld)-(%ld,%ld)\n", i, e.x1, e.y1, e.x2, e.y2);
                OutputDebugStringA(dbg_buf);
            }
        }
    } else {
        g_workspace_contour = create_edge_list(0);
        OutputDebugStringA("Workspace Contour: No monitors or 0 edges.\n");
    }
}

void CleanupGlobalContourResources() {
    if (g_desktop_contour != NULL) {
        edge_list_free(g_desktop_contour);
        g_desktop_contour = NULL;
    }
    if (g_workspace_contour != NULL) {
        edge_list_free(g_workspace_contour);
        g_workspace_contour = NULL;
    }
    FreeGlobalMonitorRectArrays();
}


void ToggleWrapEnabled(HWND hwnd)
{
    wrapEnabled = !wrapEnabled;
    if (wrapEnabled)
    {
        UpdateMonitorContours(); // Renamed: Calculate contours when enabling
        SetTimer(hwnd, IDT_WRAP_TIMER, WRAP_DELAY, NULL);
    }
    else
    {
        KillTimer(hwnd, IDT_WRAP_TIMER);
    }
}

// How many pixels from a contour edge the cursor must be to trigger a wrap.
// 1 px means the cursor must be directly on or adjacent to the boundary.
#define PIXEL_TOLERANCE 1
// How many pixels inward from the opposite contour edge to place the cursor
// after wrapping.  Must be > PIXEL_TOLERANCE to avoid immediately re-triggering.
#define WRAP_OFFSET 5

BOOL IsPointNearEdge(POINT pt, me_Edge edge, int tolerance) {
    if (edge.x1 == edge.x2) { 
        if (abs(pt.x - edge.x1) <= tolerance) { 
            return (pt.y >= edge.y1 && pt.y <= edge.y2);
        }
    } else if (edge.y1 == edge.y2) { 
        if (abs(pt.y - edge.y1) <= tolerance) { 
            return (pt.x >= edge.x1 && pt.x <= edge.x2);
        }
    }
    return FALSE;
}

// Check a contour for an edge hit and wrap the cursor if found.
// Returns TRUE if a wrap was performed.
static BOOL TryWrapAgainstContour(POINT current_pos, EdgeList* contour, const char* contour_type_str)
{
    if (contour == NULL || contour->size == 0) return FALSE;

    char dbg_buf[512];

    // Calculate bounding box
    long contour_min_x = contour->edges[0].x1;
    long contour_max_x = contour->edges[0].x1;
    long contour_min_y = contour->edges[0].y1;
    long contour_max_y = contour->edges[0].y1;

    for (SIZE_T k = 0; k < contour->size; ++k) {
        me_Edge ce = contour->edges[k];
        contour_min_x = min(contour_min_x, ce.x1);
        contour_max_x = max(contour_max_x, ce.x2);
        contour_min_y = min(contour_min_y, ce.y1);
        contour_max_y = max(contour_max_y, ce.y2);
    }
    long contour_center_x = contour_min_x + (contour_max_x - contour_min_x) / 2;
    long contour_center_y = contour_min_y + (contour_max_y - contour_min_y) / 2;

    for (SIZE_T i = 0; i < contour->size; i++) {
        me_Edge hit_edge = contour->edges[i];
        if (!IsPointNearEdge(current_pos, hit_edge, PIXEL_TOLERANCE))
            continue;

        POINT new_pos = current_pos;

        sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Hit edge %zu: (%ld,%ld)-(%ld,%ld). Cursor: (%ld,%ld). Contour: %s\n",
            i, hit_edge.x1, hit_edge.y1, hit_edge.x2, hit_edge.y2,
            current_pos.x, current_pos.y, contour_type_str);
        OutputDebugStringA(dbg_buf);

        if (hit_edge.x1 == hit_edge.x2) { // Vertical edge
            BOOL is_left_ish_hit = (hit_edge.x1 < contour_center_x);
            new_pos.x = is_left_ish_hit ? (contour_max_x - WRAP_OFFSET) : (contour_min_x + WRAP_OFFSET);
            new_pos.y = max(contour_min_y, min(current_pos.y, contour_max_y));
        } else { // Horizontal edge
            BOOL is_top_ish_hit = (hit_edge.y1 < contour_center_y);
            new_pos.y = is_top_ish_hit ? (contour_max_y - WRAP_OFFSET) : (contour_min_y + WRAP_OFFSET);
            new_pos.x = max(contour_min_x, min(current_pos.x, contour_max_x));
        }

        if (new_pos.x != current_pos.x || new_pos.y != current_pos.y) {
            SetCursorPos(new_pos.x, new_pos.y);
            return TRUE;
        }
    }
    return FALSE;
}

// How many timer ticks between workspace contour checks.
// The workspace contour catches cursor stuck at taskbar edges during window
// drags.  Checking every 5th tick (~100 ms at WRAP_DELAY=20) keeps CPU low.
#define WORKSPACE_CHECK_INTERVAL 5

void WrapMouseWhileDragging()
{
    static int workspace_tick = 0;

    if (!wrapEnabled) return;

    POINT current_pos;
    GetCursorPos(&current_pos);

    // Primary: always check desktop contour
    if (TryWrapAgainstContour(current_pos, g_desktop_contour, "Desktop"))
        return;

    // Fallback: check workspace contour less frequently to handle
    // cursor stuck at taskbar / work-area boundary during window drags.
    workspace_tick++;
    if (workspace_tick >= WORKSPACE_CHECK_INTERVAL) {
        workspace_tick = 0;
        TryWrapAgainstContour(current_pos, g_workspace_contour, "Workspace");
    }
}

