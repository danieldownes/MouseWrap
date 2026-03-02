#include "unity.h"
#include "multimonitor_edges.h"
#include "multimonitor_contour.h"

/* ---- EdgeList operations ---- */

void test_edge_list_create(void) {
    EdgeList* list = create_edge_list(8);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_EQUAL_UINT(0, list->size);
    TEST_ASSERT_EQUAL_UINT(8, list->capacity);
    edge_list_free(list);
}

void test_edge_list_add(void) {
    EdgeList* list = create_edge_list(4);
    TEST_ASSERT_NOT_NULL(list);
    me_Edge e = me_create_edge(0, 10, 5, 5);
    edge_list_add(list, e);
    TEST_ASSERT_EQUAL_UINT(1, list->size);
    TEST_ASSERT_TRUE(me_edge_equals(&list->edges[0], &e));
    edge_list_free(list);
}

void test_edge_list_add_grows_capacity(void) {
    EdgeList* list = create_edge_list(2);
    TEST_ASSERT_NOT_NULL(list);
    me_Edge e1 = me_create_edge(0, 10, 0, 0);
    me_Edge e2 = me_create_edge(0, 10, 5, 5);
    me_Edge e3 = me_create_edge(0, 10, 10, 10);
    edge_list_add(list, e1);
    edge_list_add(list, e2);
    edge_list_add(list, e3); /* triggers realloc */
    TEST_ASSERT_EQUAL_UINT(3, list->size);
    TEST_ASSERT_TRUE(list->capacity >= 3);
    TEST_ASSERT_TRUE(me_edge_equals(&list->edges[2], &e3));
    edge_list_free(list);
}

void test_edge_list_remove(void) {
    EdgeList* list = create_edge_list(4);
    TEST_ASSERT_NOT_NULL(list);
    me_Edge e1 = me_create_edge(0, 10, 0, 0);
    me_Edge e2 = me_create_edge(0, 10, 5, 5);
    edge_list_add(list, e1);
    edge_list_add(list, e2);
    edge_list_remove(list, e1);
    TEST_ASSERT_EQUAL_UINT(1, list->size);
    TEST_ASSERT_TRUE(me_edge_equals(&list->edges[0], &e2));
    edge_list_free(list);
}

void test_edge_list_remove_nonexistent(void) {
    EdgeList* list = create_edge_list(4);
    TEST_ASSERT_NOT_NULL(list);
    me_Edge e1 = me_create_edge(0, 10, 0, 0);
    me_Edge e_missing = me_create_edge(99, 99, 99, 99);
    edge_list_add(list, e1);
    edge_list_remove(list, e_missing);
    TEST_ASSERT_EQUAL_UINT(1, list->size); /* unchanged */
    edge_list_free(list);
}

void test_edge_list_clear(void) {
    EdgeList* list = create_edge_list(4);
    TEST_ASSERT_NOT_NULL(list);
    edge_list_add(list, me_create_edge(0, 10, 0, 0));
    edge_list_add(list, me_create_edge(0, 10, 5, 5));
    edge_list_clear(list);
    TEST_ASSERT_EQUAL_UINT(0, list->size);
    edge_list_free(list);
}

/* ---- get_contour ---- */

void test_contour_empty_input(void) {
    EdgeList* contour = get_contour(NULL, 0);
    TEST_ASSERT_NOT_NULL(contour);
    TEST_ASSERT_EQUAL_UINT(0, contour->size);
    edge_list_free(contour);
}

void test_contour_single_rectangle(void) {
    me_Rect rects[] = { me_create_rect(0, 10, 0, 30) };
    EdgeList* contour = get_contour(rects, 1);
    TEST_ASSERT_NOT_NULL(contour);
    TEST_ASSERT_EQUAL_UINT(4, contour->size);
    edge_list_free(contour);
}

void test_contour_two_adjacent_rectangles(void) {
    /* R1 and R2 share the edge at x=10 */
    me_Rect rects[] = {
        me_create_rect(0, 10, 0, 30),
        me_create_rect(10, 15, 0, 30)
    };
    EdgeList* contour = get_contour(rects, 2);
    TEST_ASSERT_NOT_NULL(contour);
    TEST_ASSERT_EQUAL_UINT(6, contour->size);
    edge_list_free(contour);
}

void test_contour_l_shape_staggered(void) {
    /* Two diagonally placed rectangles with no shared edges */
    me_Rect rects[] = {
        me_create_rect(0, 10, 0, 10),
        me_create_rect(10, 20, 10, 20)
    };
    EdgeList* contour = get_contour(rects, 2);
    TEST_ASSERT_NOT_NULL(contour);
    TEST_ASSERT_EQUAL_UINT(8, contour->size);
    edge_list_free(contour);
}

void test_contour_t_junction(void) {
    /*
     * R1 = horizontal bar:  x=0..20, y=0..10
     * R2 = vertical stem:   x=5..15, y=10..20
     *
     * The C algorithm decomposes edges into minimal segments at every
     * unique coordinate.  R1-Top (0,20,10,10) becomes three segments:
     *   (0,5,10,10)  (5,15,10,10)  (15,20,10,10)
     * R2-Bottom (5,15,10,10) is one segment.
     * The middle R1-Top segment matches R2-Bottom → both pair-removed.
     * Total: 12 segments - 2 paired = 10 contour edges.
     */
    me_Rect rects[] = {
        me_create_rect(0, 20, 0, 10),
        me_create_rect(5, 15, 10, 20)
    };
    EdgeList* contour = get_contour(rects, 2);
    TEST_ASSERT_NOT_NULL(contour);
    TEST_ASSERT_EQUAL_UINT(10, contour->size);
    edge_list_free(contour);
}
