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

// Edges the user has disabled (no wrapping at these boundaries)
EdgeList* g_disabled_edges = NULL;
// Edges the user has set to delayed wrapping (100 ms pause then wrap)
EdgeList* g_delayed_edges = NULL;

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

    // Prune disabled/delayed edges that no longer exist in the new desktop contour
    if (g_desktop_contour != NULL) {
        EdgeList* lists[] = { g_disabled_edges, g_delayed_edges };
        for (int li = 0; li < 2; li++) {
            EdgeList* el = lists[li];
            if (el == NULL) continue;
            for (SIZE_T i = el->size; i > 0; i--) {
                me_Edge de = el->edges[i - 1];
                BOOL still_exists = FALSE;
                for (SIZE_T j = 0; j < g_desktop_contour->size; j++) {
                    if (me_edge_equals(&de, &g_desktop_contour->edges[j])) {
                        still_exists = TRUE;
                        break;
                    }
                }
                if (!still_exists)
                    edge_list_remove(el, de);
            }
        }
        SaveEdgeStates();
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
    if (g_disabled_edges != NULL) {
        edge_list_free(g_disabled_edges);
        g_disabled_edges = NULL;
    }
    if (g_delayed_edges != NULL) {
        edge_list_free(g_delayed_edges);
        g_delayed_edges = NULL;
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

// Check whether two axis-aligned edges lie on the same line and overlap.
// Used so that a disabled desktop edge also blocks the corresponding
// workspace-contour edge (which may be a sub-segment shifted by the taskbar).
static BOOL EdgesOverlap(const me_Edge* a, const me_Edge* b) {
    if (a->x1 == a->x2 && b->x1 == b->x2 && a->x1 == b->x1) {
        // Both vertical at the same x — check y-range overlap
        return (a->y1 < b->y2 && b->y1 < a->y2);
    }
    if (a->y1 == a->y2 && b->y1 == b->y2 && a->y1 == b->y1) {
        // Both horizontal at the same y — check x-range overlap
        return (a->x1 < b->x2 && b->x1 < a->x2);
    }
    return FALSE;
}

EdgeState GetEdgeState(me_Edge edge) {
    if (g_disabled_edges != NULL) {
        for (SIZE_T i = 0; i < g_disabled_edges->size; i++) {
            if (EdgesOverlap(&g_disabled_edges->edges[i], &edge))
                return EDGE_NOWRAP;
        }
    }
    if (g_delayed_edges != NULL) {
        for (SIZE_T i = 0; i < g_delayed_edges->size; i++) {
            if (EdgesOverlap(&g_delayed_edges->edges[i], &edge))
                return EDGE_DELAYED;
        }
    }
    return EDGE_WRAP;
}

void CycleEdgeState(me_Edge edge) {
    // Check disabled list first (NOWRAP → WRAP: remove from disabled)
    if (g_disabled_edges != NULL) {
        for (SIZE_T i = 0; i < g_disabled_edges->size; i++) {
            if (me_edge_equals(&g_disabled_edges->edges[i], &edge)) {
                edge_list_remove(g_disabled_edges, edge);
                SaveEdgeStates();
                return;
            }
        }
    }
    // Check delayed list (DELAYED → NOWRAP: remove from delayed, add to disabled)
    if (g_delayed_edges != NULL) {
        for (SIZE_T i = 0; i < g_delayed_edges->size; i++) {
            if (me_edge_equals(&g_delayed_edges->edges[i], &edge)) {
                edge_list_remove(g_delayed_edges, edge);
                if (g_disabled_edges == NULL)
                    g_disabled_edges = create_edge_list(4);
                edge_list_add(g_disabled_edges, edge);
                SaveEdgeStates();
                return;
            }
        }
    }
    // Not in either list (WRAP → DELAYED: add to delayed)
    if (g_delayed_edges == NULL)
        g_delayed_edges = create_edge_list(4);
    edge_list_add(g_delayed_edges, edge);
    SaveEdgeStates();
}

#define MW_REG_KEY          L"Software\\MouseWrap"
#define MW_REG_VAL_DISABLED L"DisabledEdges"
#define MW_REG_VAL_DELAYED  L"DelayedEdges"

static void SaveEdgeListToRegistry(HKEY hKey, const WCHAR* valueName, EdgeList* list) {
    if (list == NULL || list->size == 0) {
        RegDeleteValueW(hKey, valueName);
        return;
    }
    DWORD cbData = (DWORD)(list->size * sizeof(me_Edge));
    RegSetValueExW(hKey, valueName, 0, REG_BINARY,
                   (const BYTE*)list->edges, cbData);
}

static void LoadEdgeListFromRegistry(HKEY hKey, const WCHAR* valueName, EdgeList** pList) {
    DWORD cbData = 0;
    DWORD dwType = 0;
    if (RegQueryValueExW(hKey, valueName, NULL, &dwType, NULL, &cbData) != ERROR_SUCCESS
        || dwType != REG_BINARY || cbData == 0 || (cbData % sizeof(me_Edge)) != 0)
        return;
    SIZE_T count = cbData / sizeof(me_Edge);
    if (*pList != NULL)
        edge_list_free(*pList);
    *pList = create_edge_list(count);
    if (*pList == NULL) return;

    me_Edge* buf = (me_Edge*)HeapAlloc(GetProcessHeap(), 0, cbData);
    if (buf == NULL) return;
    if (RegQueryValueExW(hKey, valueName, NULL, NULL, (BYTE*)buf, &cbData) == ERROR_SUCCESS) {
        for (SIZE_T i = 0; i < count; i++)
            edge_list_add(*pList, buf[i]);
    }
    HeapFree(GetProcessHeap(), 0, buf);
}

void SaveEdgeStates(void) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, NULL, 0,
                        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return;
    SaveEdgeListToRegistry(hKey, MW_REG_VAL_DISABLED, g_disabled_edges);
    SaveEdgeListToRegistry(hKey, MW_REG_VAL_DELAYED,  g_delayed_edges);
    RegCloseKey(hKey);
}

void LoadEdgeStates(void) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return;
    LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DISABLED, &g_disabled_edges);
    LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DELAYED,  &g_delayed_edges);
    RegCloseKey(hKey);
}

// Configurable delay for EDGE_DELAYED edges
DWORD g_edge_delay_ms = 300;

#define MW_REG_VAL_DELAY_MS L"DelayMs"

void SaveDelayMs(void) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, NULL, 0,
                        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return;
    RegSetValueExW(hKey, MW_REG_VAL_DELAY_MS, 0, REG_DWORD,
                   (const BYTE*)&g_edge_delay_ms, sizeof(g_edge_delay_ms));
    RegCloseKey(hKey);
}

void LoadDelayMs(void) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return;
    DWORD cbData = sizeof(g_edge_delay_ms);
    DWORD dwType = 0;
    DWORD val = 0;
    if (RegQueryValueExW(hKey, MW_REG_VAL_DELAY_MS, NULL, &dwType, (BYTE*)&val, &cbData) == ERROR_SUCCESS
        && dwType == REG_DWORD && val >= 200 && val <= 1000) {
        g_edge_delay_ms = val;
    }
    RegCloseKey(hKey);
}

// Delay timer state for EDGE_DELAYED edges
static me_Edge g_delay_edge;
static BOOL    g_delay_active = FALSE;
static DWORD   g_delay_start  = 0;

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

        EdgeState state = GetEdgeState(hit_edge);
        if (state == EDGE_NOWRAP)
            continue;

        if (state == EDGE_DELAYED) {
            if (g_delay_active && EdgesOverlap(&hit_edge, &g_delay_edge)) {
                if (GetTickCount() - g_delay_start >= g_edge_delay_ms) {
                    g_delay_active = FALSE;
                    // fall through to wrap
                } else {
                    continue; // still waiting
                }
            } else {
                g_delay_active = TRUE;
                g_delay_edge = hit_edge;
                g_delay_start = GetTickCount();
                continue; // start waiting
            }
        }

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
        if (TryWrapAgainstContour(current_pos, g_workspace_contour, "Workspace"))
            return;
    }

    // Reset delay timer if cursor moved away from the tracked edge
    if (g_delay_active) {
        if (!IsPointNearEdge(current_pos, g_delay_edge, PIXEL_TOLERANCE))
            g_delay_active = FALSE;
    }
}

