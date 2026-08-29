#include "unity.h"
#include <windows.h>
#include <wchar.h>
#include <wctype.h>
#include "mouse_wrap.h"

/* ---- IsPointNearEdge — vertical edge ---- */

void test_near_edge_vertical_on_edge(void) {
    me_Edge edge = me_create_edge(10, 10, 0, 20);
    POINT pt = { 10, 10 };
    TEST_ASSERT_TRUE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_vertical_within_tolerance(void) {
    me_Edge edge = me_create_edge(10, 10, 0, 20);
    POINT pt = { 11, 10 };
    TEST_ASSERT_TRUE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_vertical_outside_tolerance(void) {
    me_Edge edge = me_create_edge(10, 10, 0, 20);
    POINT pt = { 12, 10 };
    TEST_ASSERT_FALSE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_vertical_outside_range(void) {
    me_Edge edge = me_create_edge(10, 10, 0, 20);
    POINT pt = { 10, 25 }; /* y beyond edge span */
    TEST_ASSERT_FALSE(IsPointNearEdge(pt, edge, 1));
}

/* ---- IsPointNearEdge — horizontal edge ---- */

void test_near_edge_horizontal_on_edge(void) {
    me_Edge edge = me_create_edge(0, 20, 10, 10);
    POINT pt = { 10, 10 };
    TEST_ASSERT_TRUE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_horizontal_within_tolerance(void) {
    me_Edge edge = me_create_edge(0, 20, 10, 10);
    POINT pt = { 10, 11 };
    TEST_ASSERT_TRUE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_horizontal_outside_tolerance(void) {
    me_Edge edge = me_create_edge(0, 20, 10, 10);
    POINT pt = { 10, 12 };
    TEST_ASSERT_FALSE(IsPointNearEdge(pt, edge, 1));
}

void test_near_edge_horizontal_outside_range(void) {
    me_Edge edge = me_create_edge(0, 20, 10, 10);
    POINT pt = { 25, 10 }; /* x beyond edge span */
    TEST_ASSERT_FALSE(IsPointNearEdge(pt, edge, 1));
}

/* ---- Zero tolerance ---- */

void test_near_edge_zero_tolerance(void) {
    me_Edge edge = me_create_edge(10, 10, 0, 20);
    POINT on  = { 10, 10 };
    POINT off = { 11, 10 };
    TEST_ASSERT_TRUE(IsPointNearEdge(on, edge, 0));
    TEST_ASSERT_FALSE(IsPointNearEdge(off, edge, 0));
}

/* ---- RemoveEdgesNotOnContour — strip taskbar-boundary edges ---- */

/* Single 1920x1080 monitor with a 40 px taskbar at the bottom. The work area
 * is 1920x1040; its bottom edge (y=1040) is the taskbar boundary and must go,
 * while the left/right/top edges lie on desktop edges and must stay. */
void test_remove_taskbar_edge_bottom_taskbar(void) {
    me_Rect desktop = me_create_rect(0, 1920, 0, 1080);
    me_Rect work    = me_create_rect(0, 1920, 0, 1040);
    EdgeList* desktop_contour = get_contour(&desktop, 1);
    EdgeList* work_contour    = get_contour(&work, 1);
    TEST_ASSERT_EQUAL_size_t(4, work_contour->size);

    RemoveEdgesNotOnContour(work_contour, desktop_contour);

    TEST_ASSERT_EQUAL_size_t(3, work_contour->size);
    for (size_t i = 0; i < work_contour->size; i++) {
        me_Edge e = work_contour->edges[i];
        TEST_ASSERT_FALSE(e.y1 == 1040 && e.y2 == 1040);
    }
    edge_list_free(desktop_contour);
    edge_list_free(work_contour);
}

/* Taskbar on the left: the work area's left edge (x=60) is the boundary. */
void test_remove_taskbar_edge_left_taskbar(void) {
    me_Rect desktop = me_create_rect(0, 1920, 0, 1080);
    me_Rect work    = me_create_rect(60, 1920, 0, 1080);
    EdgeList* desktop_contour = get_contour(&desktop, 1);
    EdgeList* work_contour    = get_contour(&work, 1);

    RemoveEdgesNotOnContour(work_contour, desktop_contour);

    TEST_ASSERT_EQUAL_size_t(3, work_contour->size);
    for (size_t i = 0; i < work_contour->size; i++) {
        me_Edge e = work_contour->edges[i];
        TEST_ASSERT_FALSE(e.x1 == 60 && e.x2 == 60);
    }
    edge_list_free(desktop_contour);
    edge_list_free(work_contour);
}

/* No taskbar: work area == desktop, nothing should be removed. */
void test_remove_taskbar_edge_no_taskbar_keeps_all(void) {
    me_Rect r = me_create_rect(0, 1920, 0, 1080);
    EdgeList* desktop_contour = get_contour(&r, 1);
    EdgeList* work_contour    = get_contour(&r, 1);

    RemoveEdgesNotOnContour(work_contour, desktop_contour);

    TEST_ASSERT_EQUAL_size_t(4, work_contour->size);
    edge_list_free(desktop_contour);
    edge_list_free(work_contour);
}

/* NULL / empty reference: every edge is off-reference and gets removed. */
void test_remove_taskbar_edge_null_reference_removes_all(void) {
    me_Rect r = me_create_rect(0, 100, 0, 100);
    EdgeList* work_contour = get_contour(&r, 1);

    RemoveEdgesNotOnContour(work_contour, NULL);

    TEST_ASSERT_EQUAL_size_t(0, work_contour->size);
    edge_list_free(work_contour);
}

/* ---- ResolveEdgeState — "While dragging" behaviour for Wrap edges ---- */

void test_resolve_not_dragging_keeps_state(void) {
    g_drag_wrap_mode = DRAG_WRAP_NONE;
    TEST_ASSERT_EQUAL_INT(EDGE_WRAP,    ResolveEdgeState(EDGE_WRAP,    FALSE));
    TEST_ASSERT_EQUAL_INT(EDGE_DELAYED, ResolveEdgeState(EDGE_DELAYED, FALSE));
    TEST_ASSERT_EQUAL_INT(EDGE_NOWRAP,  ResolveEdgeState(EDGE_NOWRAP,  FALSE));
}

void test_resolve_dragging_delayed_mode(void) {
    g_drag_wrap_mode = DRAG_WRAP_DELAYED;
    TEST_ASSERT_EQUAL_INT(EDGE_DELAYED, ResolveEdgeState(EDGE_WRAP, TRUE));
}

void test_resolve_dragging_instant_mode(void) {
    g_drag_wrap_mode = DRAG_WRAP_INSTANT;
    TEST_ASSERT_EQUAL_INT(EDGE_WRAP, ResolveEdgeState(EDGE_WRAP, TRUE));
}

void test_resolve_dragging_none_mode(void) {
    g_drag_wrap_mode = DRAG_WRAP_NONE;
    TEST_ASSERT_EQUAL_INT(EDGE_NOWRAP, ResolveEdgeState(EDGE_WRAP, TRUE));
}

/* Edges the user set to Delayed / No Wrap are never changed by the drag mode. */
void test_resolve_dragging_only_affects_wrap_edges(void) {
    g_drag_wrap_mode = DRAG_WRAP_INSTANT;
    TEST_ASSERT_EQUAL_INT(EDGE_DELAYED, ResolveEdgeState(EDGE_DELAYED, TRUE));
    TEST_ASSERT_EQUAL_INT(EDGE_NOWRAP,  ResolveEdgeState(EDGE_NOWRAP,  TRUE));
    g_drag_wrap_mode = DRAG_WRAP_NONE;
    TEST_ASSERT_EQUAL_INT(EDGE_DELAYED, ResolveEdgeState(EDGE_DELAYED, TRUE));
    g_drag_wrap_mode = DRAG_WRAP_DELAYED;
    TEST_ASSERT_EQUAL_INT(EDGE_NOWRAP,  ResolveEdgeState(EDGE_NOWRAP,  TRUE));
}

/* ---- GetLayoutId — per-layout settings key ---- */

void test_layout_id_is_16_hex_chars(void) {
    me_Rect r = me_create_rect(0, 1920, 0, 1080);
    WCHAR id[LAYOUT_ID_CCH];
    GetLayoutId(&r, 1, id, LAYOUT_ID_CCH);
    TEST_ASSERT_EQUAL_size_t(16, wcslen(id));
    for (int i = 0; i < 16; i++)
        TEST_ASSERT_TRUE(iswxdigit(id[i]));
}

void test_layout_id_is_deterministic(void) {
    me_Rect r = me_create_rect(0, 1920, 0, 1080);
    WCHAR a[LAYOUT_ID_CCH], b[LAYOUT_ID_CCH];
    GetLayoutId(&r, 1, a, LAYOUT_ID_CCH);
    GetLayoutId(&r, 1, b, LAYOUT_ID_CCH);
    TEST_ASSERT_EQUAL_INT(0, wcscmp(a, b));
}

/* Monitor enumeration order can change between sessions; the id must not. */
void test_layout_id_ignores_monitor_order(void) {
    me_Rect ab[2] = { me_create_rect(0, 1920, 0, 1080), me_create_rect(1920, 3840, 0, 1080) };
    me_Rect ba[2] = { me_create_rect(1920, 3840, 0, 1080), me_create_rect(0, 1920, 0, 1080) };
    WCHAR a[LAYOUT_ID_CCH], b[LAYOUT_ID_CCH];
    GetLayoutId(ab, 2, a, LAYOUT_ID_CCH);
    GetLayoutId(ba, 2, b, LAYOUT_ID_CCH);
    TEST_ASSERT_EQUAL_INT(0, wcscmp(a, b));
}

/* Turning a monitor off yields a different layout, so a different id. */
void test_layout_id_differs_when_monitor_removed(void) {
    me_Rect two[2] = { me_create_rect(0, 1920, 0, 1080), me_create_rect(1920, 3840, 0, 1080) };
    WCHAR a[LAYOUT_ID_CCH], b[LAYOUT_ID_CCH];
    GetLayoutId(two, 2, a, LAYOUT_ID_CCH);
    GetLayoutId(two, 1, b, LAYOUT_ID_CCH);
    TEST_ASSERT_NOT_EQUAL(0, wcscmp(a, b));
}

void test_layout_id_differs_for_different_geometry(void) {
    me_Rect r1 = me_create_rect(0, 1920, 0, 1080);
    me_Rect r2 = me_create_rect(0, 2560, 0, 1440);
    WCHAR a[LAYOUT_ID_CCH], b[LAYOUT_ID_CCH];
    GetLayoutId(&r1, 1, a, LAYOUT_ID_CCH);
    GetLayoutId(&r2, 1, b, LAYOUT_ID_CCH);
    TEST_ASSERT_NOT_EQUAL(0, wcscmp(a, b));
}

void test_layout_id_small_buffer_is_empty_string(void) {
    me_Rect r = me_create_rect(0, 1920, 0, 1080);
    WCHAR id[4] = L"xyz";
    GetLayoutId(&r, 1, id, 4);
    TEST_ASSERT_EQUAL_INT(0, id[0]);
}
