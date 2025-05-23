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
BOOL g_use_workspace_contour = FALSE; // FALSE = Desktop, TRUE = Workspace. Default to Desktop.

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

#define PIXEL_TOLERANCE 1
#define WRAP_OFFSET 5 // Increased offset for placing cursor after wrap

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

// --- START OF REPLACEMENT FOR WrapMouseWhileDragging ---
void WrapMouseWhileDragging()
{
    EdgeList* contour_to_use = NULL;
    me_Rect* monitor_rects_array = NULL;
    SIZE_T monitor_count = 0;
    const char* contour_type_str = "";

    if (g_use_workspace_contour) {
        if (!wrapEnabled || g_workspace_contour == NULL || g_workspace_contour->size == 0) {
            return;
        }
        contour_to_use = g_workspace_contour;
        monitor_rects_array = g_monitor_rects_workspace;
        monitor_count = g_monitor_count_workspace;
        contour_type_str = "Workspace";
    } else {
        if (!wrapEnabled || g_desktop_contour == NULL || g_desktop_contour->size == 0) {
            return;
        }
        contour_to_use = g_desktop_contour;
        monitor_rects_array = g_monitor_rects_desktop;
        monitor_count = g_monitor_count_desktop;
        contour_type_str = "Desktop";
    }
    
    if (contour_to_use == NULL || contour_to_use->size == 0) return; // Should be caught above, but defensive

    POINT current_pos;
    GetCursorPos(&current_pos);

    // Calculate bounding box of the contour_to_use
    long contour_min_x = contour_to_use->edges[0].x1;
    long contour_max_x = contour_to_use->edges[0].x1;
    long contour_min_y = contour_to_use->edges[0].y1;
    long contour_max_y = contour_to_use->edges[0].y1;

    for (SIZE_T k = 0; k < contour_to_use->size; ++k) {
        me_Edge ce = contour_to_use->edges[k];
        contour_min_x = min(contour_min_x, ce.x1);
        contour_max_x = max(contour_max_x, ce.x2);
        contour_min_y = min(contour_min_y, ce.y1);
        contour_max_y = max(contour_max_y, ce.y2);
    }
    long contour_center_x = contour_min_x + (contour_max_x - contour_min_x) / 2;
    long contour_center_y = contour_min_y + (contour_max_y - contour_min_y) / 2;

    for (SIZE_T i = 0; i < contour_to_use->size; i++) {
        me_Edge hit_edge = contour_to_use->edges[i];
        BOOL on_edge_vicinity = IsPointNearEdge(current_pos, hit_edge, PIXEL_TOLERANCE);
        
        POINT new_pos = current_pos;
        BOOL wrap_logic_applied = FALSE; 

        if (on_edge_vicinity) {
            char dbg_buf[512];
            sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Hit edge %zu: (%ld,%ld)-(%ld,%ld). Cursor: (%ld,%ld). Contour: %s\n",
                i, hit_edge.x1, hit_edge.y1, hit_edge.x2, hit_edge.y2,
                current_pos.x, current_pos.y, contour_type_str);
            OutputDebugStringA(dbg_buf);

            sprintf_s(dbg_buf, sizeof(dbg_buf), "ContourBB: (%ld,%ld)-(%ld,%ld). Center: (%ld,%ld)\n",
                contour_min_x, contour_min_y, contour_max_x, contour_max_y,
                contour_center_x, contour_center_y);
            OutputDebugStringA(dbg_buf);

            // Specific 2-monitor vertical wrapping logic
            if (hit_edge.x1 == hit_edge.x2 && monitor_count == 2) {
                me_Rect m_left_rect, m_right_rect;
                // Determine which monitor is physically to the left and which is to the right
                if (monitor_rects_array[0].xMin < monitor_rects_array[1].xMin) {
                    m_left_rect = monitor_rects_array[0];
                    m_right_rect = monitor_rects_array[1];
                } else {
                    m_left_rect = monitor_rects_array[1];
                    m_right_rect = monitor_rects_array[0];
                }
                sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon V. M_Left:(%ld,%ld R:%ld B:%ld), M_Right:(%ld,%ld R:%ld B:%ld)\n",
                    m_left_rect.xMin, m_left_rect.yMin, m_left_rect.xMax, m_left_rect.yMax,
                    m_right_rect.xMin, m_right_rect.yMin, m_right_rect.xMax, m_right_rect.yMax);
                OutputDebugStringA(dbg_buf);

                // Define conceptual edges of the individual monitors
                me_Edge m_left_rect_L_edge = me_create_edge(m_left_rect.xMin, m_left_rect.xMin, m_left_rect.yMin, m_left_rect.yMax);
                me_Edge m_left_rect_R_edge = me_create_edge(m_left_rect.xMax, m_left_rect.xMax, m_left_rect.yMin, m_left_rect.yMax);
                me_Edge m_right_rect_L_edge = me_create_edge(m_right_rect.xMin, m_right_rect.xMin, m_right_rect.yMin, m_right_rect.yMax);
                me_Edge m_right_rect_R_edge = me_create_edge(m_right_rect.xMax, m_right_rect.xMax, m_right_rect.yMin, m_right_rect.yMax);

                // Check for internal pass-through condition
                if (m_left_rect.xMax == m_right_rect.xMin) { // Monitors are perfectly adjacent
                    BOOL is_pass_through = FALSE;
                    // Case 1: Cursor hits right edge of left monitor, check if it can pass into right monitor
                    if (me_edge_equals(&hit_edge, &m_left_rect_R_edge) &&
                        (current_pos.y >= m_right_rect.yMin && current_pos.y <= m_right_rect.yMax)) {
                        is_pass_through = TRUE;
                    }
                    // Case 2: Cursor hits left edge of right monitor, check if it can pass into left monitor
                    else if (me_edge_equals(&hit_edge, &m_right_rect_L_edge) &&
                             (current_pos.y >= m_left_rect.yMin && current_pos.y <= m_left_rect.yMax)) {
                        is_pass_through = TRUE;
                    }

                    if (is_pass_through) {
                        sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon Internal Pass-through at X=%ld. No teleport.\n", m_left_rect.xMax);
                        OutputDebugStringA(dbg_buf);
                        return; // Exit: cursor passes naturally, no SetCursorPos needed for this event
                    }
                }

                // If not a pass-through, apply standard 2-monitor exterior wrapping logic
                if (me_edge_equals(&hit_edge, &m_left_rect_L_edge)) { // Hit M_Left's Left edge
                    new_pos.x = m_right_rect.xMax - WRAP_OFFSET;
                    new_pos.y = max(m_right_rect.yMin, min(current_pos.y, m_right_rect.yMax));
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon Case: M_Left_L to M_Right_R. NewPos:(%ld,%ld)\n", new_pos.x, new_pos.y); OutputDebugStringA(dbg_buf);
                    wrap_logic_applied = TRUE;
                } else if (me_edge_equals(&hit_edge, &m_left_rect_R_edge)) { // Hit M_Left's Right edge (and not pass-through)
                    new_pos.x = m_right_rect.xMin + WRAP_OFFSET;
                    new_pos.y = max(m_right_rect.yMin, min(current_pos.y, m_right_rect.yMax));
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon Case: M_Left_R to M_Right_L. NewPos:(%ld,%ld)\n", new_pos.x, new_pos.y); OutputDebugStringA(dbg_buf);
                    wrap_logic_applied = TRUE;
                } else if (me_edge_equals(&hit_edge, &m_right_rect_L_edge)) { // Hit M_Right's Left edge (and not pass-through)
                    new_pos.x = m_left_rect.xMax - WRAP_OFFSET;
                    new_pos.y = max(m_left_rect.yMin, min(current_pos.y, m_left_rect.yMax));
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon Case: M_Right_L to M_Left_R. NewPos:(%ld,%ld)\n", new_pos.x, new_pos.y); OutputDebugStringA(dbg_buf);
                    wrap_logic_applied = TRUE;
                } else if (me_edge_equals(&hit_edge, &m_right_rect_R_edge)) { // Hit M_Right's Right edge
                    new_pos.x = m_left_rect.xMin + WRAP_OFFSET;
                    new_pos.y = max(m_left_rect.yMin, min(current_pos.y, m_left_rect.yMax));
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: 2Mon Case: M_Right_R to M_Left_L. NewPos:(%ld,%ld)\n", new_pos.x, new_pos.y); OutputDebugStringA(dbg_buf);
                    wrap_logic_applied = TRUE;
                }
            }
            
            if (!wrap_logic_applied) { // General heuristic or horizontal edge or not 2 monitors for V-edge, or 2-mon logic decided not to apply (e.g. edge didn't match)
                 if (hit_edge.x1 == hit_edge.x2) { // Hit a vertical edge (general heuristic)
                    BOOL is_left_ish_hit = (hit_edge.x1 < contour_center_x);
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Vertical hit (gen heuristic). IsLeft-ish: %s.\n", is_left_ish_hit ? "TRUE" : "FALSE"); OutputDebugStringA(dbg_buf);
                    for (SIZE_T j = 0; j < contour_to_use->size; j++) {
                        if (i == j) continue; 
                        me_Edge candidate_edge = contour_to_use->edges[j];
                        if (candidate_edge.x1 == candidate_edge.x2) { 
                            BOOL is_candidate_opposite_side = (is_left_ish_hit && candidate_edge.x1 > contour_center_x) ||
                                                              (!is_left_ish_hit && candidate_edge.x1 < contour_center_x);
                            if (is_candidate_opposite_side) {
                                if (current_pos.y >= candidate_edge.y1 && current_pos.y <= candidate_edge.y2) {
                                    new_pos.x = candidate_edge.x1 + (is_left_ish_hit ? -WRAP_OFFSET : WRAP_OFFSET);
                                    wrap_logic_applied = TRUE;
                                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Found V candidate (gen) %zu. NewX: %ld\n", j, new_pos.x); OutputDebugStringA(dbg_buf);
                                    break;
                                }
                            }
                        }
                    }
                    if (!wrap_logic_applied) {
                        new_pos.x = is_left_ish_hit ? (contour_max_x - WRAP_OFFSET) : (contour_min_x + WRAP_OFFSET);
                        new_pos.y = max(contour_min_y, min(current_pos.y, contour_max_y));
                        sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: V Fallback (gen). NewX: %ld, NewY: %ld\n", new_pos.x, new_pos.y);
                        OutputDebugStringA(dbg_buf);
                        wrap_logic_applied = TRUE; // Fallback is also an applied logic
                    }
                } else { // Hit a horizontal edge (hit_edge.y1 == hit_edge.y2)
                    BOOL is_top_edge = (abs(hit_edge.y1 - contour_min_y) <= PIXEL_TOLERANCE);
                    BOOL is_bottom_edge = (abs(hit_edge.y1 - contour_max_y) <= PIXEL_TOLERANCE);
                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: H Hit Details. Edge:(%ld,%ld)-(%ld,%ld). Cursor:(%ld,%ld). IsTopEdge: %s. IsBottomEdge: %s\n",
                        hit_edge.x1, hit_edge.y1, hit_edge.x2, hit_edge.y2, current_pos.x, current_pos.y, is_top_edge ? "TRUE" : "FALSE", is_bottom_edge ? "TRUE" : "FALSE");
                    OutputDebugStringA(dbg_buf);

                    for (SIZE_T j = 0; j < contour_to_use->size; j++) {
                         if (i == j) continue;
                        me_Edge candidate_edge = contour_to_use->edges[j];
                        if (candidate_edge.y1 == candidate_edge.y2) { // Is candidate horizontal?
                            BOOL is_candidate_top_edge = (abs(candidate_edge.y1 - contour_min_y) <= PIXEL_TOLERANCE);
                            BOOL is_candidate_bottom_edge = (abs(candidate_edge.y1 - contour_max_y) <= PIXEL_TOLERANCE);

                            if ((is_top_edge && is_candidate_bottom_edge) || (is_bottom_edge && is_candidate_top_edge)) {
                                sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: H Candidate %zu:(%ld,%ld)-(%ld,%ld) considered. CursorX:%ld in [%ld,%ld]?\n",
                                    j, candidate_edge.x1, candidate_edge.y1, candidate_edge.x2, candidate_edge.y2,
                                    current_pos.x, candidate_edge.x1, candidate_edge.x2);
                                OutputDebugStringA(dbg_buf);
                                if (current_pos.x >= candidate_edge.x1 && current_pos.x <= candidate_edge.x2) {
                                    new_pos.y = candidate_edge.y1 + (is_top_edge ? -WRAP_OFFSET : WRAP_OFFSET);
                                    // new_pos.x remains current_pos.x
                                    wrap_logic_applied = TRUE;
                                    sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Found H candidate (gen) %zu. Edge:(%ld,%ld)-(%ld,%ld). NewY: %ld. CursorX kept: %ld\n",
                                        j, candidate_edge.x1, candidate_edge.y1, candidate_edge.x2, candidate_edge.y2, new_pos.y, current_pos.x);
                                    OutputDebugStringA(dbg_buf);
                                    break;
                                }
                            }
                        }
                    }
                    if (!wrap_logic_applied) {
                        sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: H No candidate found. Applying Fallback. IsTopEdge:%s. IsBottomEdge:%s. ContourMin/Max Y:(%ld,%ld). CursorX:%ld. ContourMin/Max X:(%ld,%ld)\n",
                            is_top_edge ? "TRUE" : "FALSE", is_bottom_edge ? "TRUE" : "FALSE", contour_min_y, contour_max_y, current_pos.x, contour_min_x, contour_max_x);
                        OutputDebugStringA(dbg_buf);
                        new_pos.y = is_top_edge ? (contour_max_y - WRAP_OFFSET) : (contour_min_y + WRAP_OFFSET);
                        new_pos.x = max(contour_min_x, min(current_pos.x, contour_max_x));
                        sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: H Fallback Result. NewPos:(%ld,%ld)\n", new_pos.x, new_pos.y);
                        OutputDebugStringA(dbg_buf);
                        wrap_logic_applied = TRUE; // Fallback is also an applied logic
                    }
                }
            }
            
            sprintf_s(dbg_buf, sizeof(dbg_buf), "Wrap: Final NewPos after H/V logic: (%ld,%ld)\n", new_pos.x, new_pos.y);
            OutputDebugStringA(dbg_buf);

            if (new_pos.x != current_pos.x || new_pos.y != current_pos.y) {
                 SetCursorPos(new_pos.x, new_pos.y);
            }
            return;
        }
    }
}

