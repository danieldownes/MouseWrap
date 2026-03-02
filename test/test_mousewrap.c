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
