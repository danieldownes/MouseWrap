#include "unity.h"
#include <windows.h>
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
