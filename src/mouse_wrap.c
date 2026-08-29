#include <windows.h>
#include "mouse_wrap.h"
#include "multimonitor_edges.h"
#include "multimonitor_contour.h"
#include <stdio.h> // For temporary debugging
#include <stdlib.h>

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
// Id of the layout whose edge states are currently loaded ("" = none yet)
static WCHAR g_layout_id[LAYOUT_ID_CCH] = L"";

BOOL wrapEnabled = TRUE;

typedef struct {
    POINT pt;
    BOOL isDragging; 
} MouseData;

// Check whether two axis-aligned edges lie on the same line and overlap.
// Used so that a disabled desktop edge also blocks the corresponding
// workspace-contour edge (which may be a sub-segment shifted by the taskbar),
// and to identify workspace edges that sit on the taskbar boundary.
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

// Remove every edge in list that does not lie on some edge of reference.
// Applied to the workspace contour with the desktop contour as reference:
// the only workspace edges that are NOT on a desktop edge are the ones where
// the work area meets the taskbar, and we never want to wrap across those.
void RemoveEdgesNotOnContour(EdgeList* list, const EdgeList* reference) {
    if (list == NULL) return;
    SIZE_T i = 0;
    while (i < list->size) {
        BOOL on_reference = FALSE;
        if (reference != NULL) {
            for (SIZE_T j = 0; j < reference->size; j++) {
                if (EdgesOverlap(&list->edges[i], &reference->edges[j])) {
                    on_reference = TRUE;
                    break;
                }
            }
        }
        if (on_reference)
            i++;
        else
            edge_list_remove(list, list->edges[i]);
    }
}

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

    // Switch to the edge states saved for this monitor layout.  Turning a
    // monitor off yields a different layout (with its own states); when the
    // original arrangement returns, its states are restored untouched.
    {
        WCHAR id[LAYOUT_ID_CCH];
        GetLayoutId(g_monitor_rects_desktop, g_monitor_count_desktop, id, LAYOUT_ID_CCH);
        if (wcscmp(id, g_layout_id) != 0) {
            wcscpy_s(g_layout_id, LAYOUT_ID_CCH, id);
            LoadEdgeStates();
        }
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
        // Drop the edges where the work area meets the taskbar — no wrapping there.
        RemoveEdgesNotOnContour(g_workspace_contour, g_desktop_contour);
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
#define MW_REG_LAYOUTS_KEY  L"Software\\MouseWrap\\Layouts"
#define MW_REG_VAL_DISABLED L"DisabledEdges"
#define MW_REG_VAL_DELAYED  L"DelayedEdges"
#define MW_REG_VAL_MONITORS L"Monitors"

static int CompareRects(const void* a, const void* b) {
    const me_Rect* ra = (const me_Rect*)a;
    const me_Rect* rb = (const me_Rect*)b;
    if (ra->xMin != rb->xMin) return ra->xMin < rb->xMin ? -1 : 1;
    if (ra->yMin != rb->yMin) return ra->yMin < rb->yMin ? -1 : 1;
    if (ra->xMax != rb->xMax) return ra->xMax < rb->xMax ? -1 : 1;
    if (ra->yMax != rb->yMax) return ra->yMax < rb->yMax ? -1 : 1;
    return 0;
}

void GetLayoutId(const me_Rect* rects, SIZE_T count, WCHAR* out, size_t cch) {
    if (out == NULL || cch == 0) return;
    out[0] = 0;
    if (cch < LAYOUT_ID_CCH) return;

    // FNV-1a 64-bit over the sorted rectangles so monitor enumeration order
    // does not matter.
    unsigned long long h = 1469598103934665603ULL;
    me_Rect* sorted = NULL;
    if (rects != NULL && count > 0) {
        sorted = (me_Rect*)HeapAlloc(GetProcessHeap(), 0, count * sizeof(me_Rect));
        if (sorted == NULL) return;
        memcpy(sorted, rects, count * sizeof(me_Rect));
        qsort(sorted, count, sizeof(me_Rect), CompareRects);
        const unsigned char* bytes = (const unsigned char*)sorted;
        for (SIZE_T i = 0; i < count * sizeof(me_Rect); i++) {
            h ^= bytes[i];
            h *= 1099511628211ULL;
        }
        HeapFree(GetProcessHeap(), 0, sorted);
    }
    swprintf_s(out, cch, L"%016llX", h);
}

// Open (or create) the registry key holding the current layout's edge states.
static HKEY OpenLayoutKey(REGSAM sam, BOOL create) {
    if (g_layout_id[0] == 0) return NULL;
    WCHAR path[128];
    swprintf_s(path, 128, L"%s\\%s", MW_REG_LAYOUTS_KEY, g_layout_id);
    HKEY hKey = NULL;
    LSTATUS st = create
        ? RegCreateKeyExW(HKEY_CURRENT_USER, path, 0, NULL, 0, sam, NULL, &hKey, NULL)
        : RegOpenKeyExW(HKEY_CURRENT_USER, path, 0, sam, &hKey);
    return (st == ERROR_SUCCESS) ? hKey : NULL;
}

// Human-readable description of the current layout, stored alongside the
// edge states so the registry is understandable when looking at it by hand.
static void SaveLayoutDescription(HKEY hKey) {
    WCHAR desc[MAX_PATH * 2] = L"";
    size_t len = 0;
    for (SIZE_T i = 0; i < g_monitor_count_desktop && len < MAX_PATH * 2 - 48; i++) {
        const me_Rect* r = &g_monitor_rects_desktop[i];
        len += (size_t)swprintf_s(desc + len, MAX_PATH * 2 - len, L"%s%d,%d,%d,%d",
                                  i ? L";" : L"", r->xMin, r->yMin, r->xMax, r->yMax);
    }
    RegSetValueExW(hKey, MW_REG_VAL_MONITORS, 0, REG_SZ,
                   (const BYTE*)desc, (DWORD)((len + 1) * sizeof(WCHAR)));
}

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
    HKEY hKey = OpenLayoutKey(KEY_SET_VALUE, TRUE);
    if (hKey == NULL) return;
    SaveEdgeListToRegistry(hKey, MW_REG_VAL_DISABLED, g_disabled_edges);
    SaveEdgeListToRegistry(hKey, MW_REG_VAL_DELAYED,  g_delayed_edges);
    SaveLayoutDescription(hKey);
    RegCloseKey(hKey);
}

void LoadEdgeStates(void) {
    if (g_layout_id[0] == 0) return; // no layout known yet

    // Start from a clean slate: states belong to the layout, never carry over
    if (g_disabled_edges != NULL) { edge_list_free(g_disabled_edges); g_disabled_edges = NULL; }
    if (g_delayed_edges  != NULL) { edge_list_free(g_delayed_edges);  g_delayed_edges  = NULL; }

    HKEY hKey = OpenLayoutKey(KEY_QUERY_VALUE, FALSE);
    if (hKey != NULL) {
        LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DISABLED, &g_disabled_edges);
        LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DELAYED,  &g_delayed_edges);
        RegCloseKey(hKey);
        return;
    }

    // Nothing saved for this layout.  Migrate the pre-4.2 global values (if
    // any) to it once, so existing users keep their settings.
    if (RegOpenKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return;
    LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DISABLED, &g_disabled_edges);
    LoadEdgeListFromRegistry(hKey, MW_REG_VAL_DELAYED,  &g_delayed_edges);
    BOOL migrated = (g_disabled_edges && g_disabled_edges->size) || (g_delayed_edges && g_delayed_edges->size);
    if (migrated) {
        RegDeleteValueW(hKey, MW_REG_VAL_DISABLED);
        RegDeleteValueW(hKey, MW_REG_VAL_DELAYED);
    }
    RegCloseKey(hKey);
    if (migrated)
        SaveEdgeStates();
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

// Behaviour of EDGE_WRAP edges while dragging
DragWrapMode g_drag_wrap_mode = DRAG_WRAP_DELAYED;

#define MW_REG_VAL_DRAG_MODE L"DragWrapMode"

void SaveDragWrapMode(void) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, NULL, 0,
                        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return;
    DWORD val = (DWORD)g_drag_wrap_mode;
    RegSetValueExW(hKey, MW_REG_VAL_DRAG_MODE, 0, REG_DWORD,
                   (const BYTE*)&val, sizeof(val));
    RegCloseKey(hKey);
}

void LoadDragWrapMode(void) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, MW_REG_KEY, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return;
    DWORD cbData = sizeof(DWORD);
    DWORD dwType = 0;
    DWORD val = 0;
    if (RegQueryValueExW(hKey, MW_REG_VAL_DRAG_MODE, NULL, &dwType, (BYTE*)&val, &cbData) == ERROR_SUCCESS
        && dwType == REG_DWORD && val <= DRAG_WRAP_NONE) {
        g_drag_wrap_mode = (DragWrapMode)val;
    }
    RegCloseKey(hKey);
}

BOOL IsPrimaryButtonDown(void) {
    // GetAsyncKeyState reports physical buttons, so honour a swapped mouse.
    int vk = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

EdgeState ResolveEdgeState(EdgeState configured, BOOL dragging) {
    if (!dragging || configured != EDGE_WRAP)
        return configured;
    switch (g_drag_wrap_mode) {
        case DRAG_WRAP_DELAYED: return EDGE_DELAYED;
        case DRAG_WRAP_NONE:    return EDGE_NOWRAP;
        case DRAG_WRAP_INSTANT:
        default:                return EDGE_WRAP;
    }
}

// Delay timer state for EDGE_DELAYED edges
static me_Edge g_delay_edge;
static BOOL    g_delay_active = FALSE;
static DWORD   g_delay_start  = 0;

// Check a contour for an edge hit and wrap the cursor if found.
// Returns TRUE if a wrap was performed.
static BOOL TryWrapAgainstContour(POINT current_pos, EdgeList* contour, const char* contour_type_str, BOOL dragging)
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

        EdgeState state = ResolveEdgeState(GetEdgeState(hit_edge), dragging);
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
// The workspace contour (with its taskbar-boundary edges removed) is a
// fallback for a cursor stuck on a work-area edge during window drags.
// Checking every 5th tick (~100 ms at WRAP_DELAY=20) keeps CPU low.
#define WORKSPACE_CHECK_INTERVAL 5

void WrapMouseWhileDragging()
{
    static int workspace_tick = 0;

    if (!wrapEnabled) return;

    POINT current_pos;
    GetCursorPos(&current_pos);
    BOOL dragging = IsPrimaryButtonDown();

    // Primary: always check desktop contour
    if (TryWrapAgainstContour(current_pos, g_desktop_contour, "Desktop", dragging))
        return;

    // Fallback: check workspace contour less frequently to handle a cursor
    // stuck on a work-area edge during window drags.  Edges along the taskbar
    // were stripped in UpdateMonitorContours, so this never wraps across them.
    workspace_tick++;
    if (workspace_tick >= WORKSPACE_CHECK_INTERVAL) {
        workspace_tick = 0;
        if (TryWrapAgainstContour(current_pos, g_workspace_contour, "Workspace", dragging))
            return;
    }

    // Reset delay timer if cursor moved away from the tracked edge
    if (g_delay_active) {
        if (!IsPointNearEdge(current_pos, g_delay_edge, PIXEL_TOLERANCE))
            g_delay_active = FALSE;
    }
}

