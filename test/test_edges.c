#include "unity.h"
#include "multimonitor_edges.h"

/* ---- me_create_edge normalization ---- */

void test_create_edge_normalizes_vertical_y_swap(void) {
    me_Edge e = me_create_edge(5, 5, 10, 0); /* y1 > y2 */
    TEST_ASSERT_EQUAL_INT(5, e.x1);
    TEST_ASSERT_EQUAL_INT(5, e.x2);
    TEST_ASSERT_EQUAL_INT(0, e.y1);
    TEST_ASSERT_EQUAL_INT(10, e.y2);
}

void test_create_edge_normalizes_horizontal_x_swap(void) {
    me_Edge e = me_create_edge(10, 0, 5, 5); /* x1 > x2 */
    TEST_ASSERT_EQUAL_INT(0, e.x1);
    TEST_ASSERT_EQUAL_INT(10, e.x2);
    TEST_ASSERT_EQUAL_INT(5, e.y1);
    TEST_ASSERT_EQUAL_INT(5, e.y2);
}

void test_create_edge_already_normalized(void) {
    me_Edge e = me_create_edge(0, 10, 0, 0);
    TEST_ASSERT_EQUAL_INT(0, e.x1);
    TEST_ASSERT_EQUAL_INT(10, e.x2);
    TEST_ASSERT_EQUAL_INT(0, e.y1);
    TEST_ASSERT_EQUAL_INT(0, e.y2);
}

/* ---- me_edge_equals ---- */

void test_edge_equals_identical(void) {
    me_Edge a = me_create_edge(0, 10, 5, 5);
    me_Edge b = me_create_edge(0, 10, 5, 5);
    TEST_ASSERT_TRUE(me_edge_equals(&a, &b));
}

void test_edge_equals_reversed(void) {
    me_Edge a = me_create_edge(0, 10, 5, 5);
    me_Edge b = me_create_edge(10, 0, 5, 5); /* reversed x */
    TEST_ASSERT_TRUE(me_edge_equals(&a, &b));
}

void test_edge_equals_null(void) {
    me_Edge a = me_create_edge(0, 10, 5, 5);
    TEST_ASSERT_FALSE(me_edge_equals(&a, NULL));
    TEST_ASSERT_FALSE(me_edge_equals(NULL, &a));
}

void test_edge_equals_different(void) {
    me_Edge a = me_create_edge(0, 10, 5, 5);
    me_Edge b = me_create_edge(0, 10, 6, 6);
    TEST_ASSERT_FALSE(me_edge_equals(&a, &b));
}

/* ---- me_edge_hash ---- */

void test_edge_hash_same_for_equal_edges(void) {
    me_Edge a = me_create_edge(0, 10, 5, 5);
    me_Edge b = me_create_edge(0, 10, 5, 5);
    TEST_ASSERT_EQUAL_UINT(me_edge_hash(&a), me_edge_hash(&b));
}

/* ---- me_edge_intersects (ported from C# EdgeTests.cs) ---- */

void test_edge_intersects_vertical_overlapping(void) {
    me_Edge e1 = me_create_edge(5, 5, 0, 10);
    me_Edge e2 = me_create_edge(5, 5, 5, 15);
    TEST_ASSERT_TRUE(me_edge_intersects(&e1, &e2));
    TEST_ASSERT_TRUE(me_edge_intersects(&e2, &e1));
}

void test_edge_intersects_horizontal_within(void) {
    me_Edge e1 = me_create_edge(0, 10, 5, 5);
    me_Edge e2 = me_create_edge(5, 15, 5, 5);
    TEST_ASSERT_TRUE(me_edge_intersects(&e1, &e2));
    TEST_ASSERT_TRUE(me_edge_intersects(&e2, &e1));
}

void test_edge_intersects_horizontal_overlap(void) {
    me_Edge e1 = me_create_edge(0, 20, 5, 5);
    me_Edge e2 = me_create_edge(5, 15, 5, 5);
    TEST_ASSERT_TRUE(me_edge_intersects(&e1, &e2));
    TEST_ASSERT_TRUE(me_edge_intersects(&e2, &e1));
}

void test_edge_intersects_same_corner(void) {
    me_Edge e1 = me_create_edge(0, 15, 5, 5);
    me_Edge e2 = me_create_edge(5, 15, 5, 5);
    TEST_ASSERT_TRUE(me_edge_intersects(&e1, &e2));
    TEST_ASSERT_TRUE(me_edge_intersects(&e2, &e1));
}

void test_edge_intersects_non_intersecting(void) {
    me_Edge e1 = me_create_edge(0, 10, 0, 10);
    me_Edge e2 = me_create_edge(20, 30, 20, 30);
    TEST_ASSERT_FALSE(me_edge_intersects(&e1, &e2));
    TEST_ASSERT_FALSE(me_edge_intersects(&e2, &e1));
}

/* ---- me_create_rect / me_rect_get_edges ---- */

void test_create_rect(void) {
    me_Rect r = me_create_rect(0, 10, 0, 20);
    TEST_ASSERT_EQUAL_INT(0, r.xMin);
    TEST_ASSERT_EQUAL_INT(10, r.xMax);
    TEST_ASSERT_EQUAL_INT(0, r.yMin);
    TEST_ASSERT_EQUAL_INT(20, r.yMax);
}

void test_rect_get_edges(void) {
    me_Rect r = me_create_rect(0, 10, 0, 20);
    me_Edge edges[4];
    me_rect_get_edges(&r, edges);

    /* Left edge: x=0, y=0..20 */
    TEST_ASSERT_EQUAL_INT(0, edges[0].x1);
    TEST_ASSERT_EQUAL_INT(0, edges[0].x2);
    TEST_ASSERT_EQUAL_INT(0, edges[0].y1);
    TEST_ASSERT_EQUAL_INT(20, edges[0].y2);

    /* Right edge: x=10, y=0..20 */
    TEST_ASSERT_EQUAL_INT(10, edges[1].x1);
    TEST_ASSERT_EQUAL_INT(10, edges[1].x2);
    TEST_ASSERT_EQUAL_INT(0, edges[1].y1);
    TEST_ASSERT_EQUAL_INT(20, edges[1].y2);

    /* Bottom edge: y=0, x=0..10 */
    TEST_ASSERT_EQUAL_INT(0, edges[2].x1);
    TEST_ASSERT_EQUAL_INT(10, edges[2].x2);
    TEST_ASSERT_EQUAL_INT(0, edges[2].y1);
    TEST_ASSERT_EQUAL_INT(0, edges[2].y2);

    /* Top edge: y=20, x=0..10 */
    TEST_ASSERT_EQUAL_INT(0, edges[3].x1);
    TEST_ASSERT_EQUAL_INT(10, edges[3].x2);
    TEST_ASSERT_EQUAL_INT(20, edges[3].y1);
    TEST_ASSERT_EQUAL_INT(20, edges[3].y2);
}
