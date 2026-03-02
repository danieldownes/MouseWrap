#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

/* --- test_edges.c --- */
extern void test_create_edge_normalizes_vertical_y_swap(void);
extern void test_create_edge_normalizes_horizontal_x_swap(void);
extern void test_create_edge_already_normalized(void);
extern void test_edge_equals_identical(void);
extern void test_edge_equals_reversed(void);
extern void test_edge_equals_null(void);
extern void test_edge_equals_different(void);
extern void test_edge_hash_same_for_equal_edges(void);
extern void test_edge_intersects_vertical_overlapping(void);
extern void test_edge_intersects_horizontal_within(void);
extern void test_edge_intersects_horizontal_overlap(void);
extern void test_edge_intersects_same_corner(void);
extern void test_edge_intersects_non_intersecting(void);
extern void test_create_rect(void);
extern void test_rect_get_edges(void);

/* --- test_contour.c --- */
extern void test_edge_list_create(void);
extern void test_edge_list_add(void);
extern void test_edge_list_add_grows_capacity(void);
extern void test_edge_list_remove(void);
extern void test_edge_list_remove_nonexistent(void);
extern void test_edge_list_clear(void);
extern void test_contour_empty_input(void);
extern void test_contour_single_rectangle(void);
extern void test_contour_two_adjacent_rectangles(void);
extern void test_contour_l_shape_staggered(void);
extern void test_contour_t_junction(void);

/* --- test_mousewrap.c --- */
extern void test_near_edge_vertical_on_edge(void);
extern void test_near_edge_vertical_within_tolerance(void);
extern void test_near_edge_vertical_outside_tolerance(void);
extern void test_near_edge_vertical_outside_range(void);
extern void test_near_edge_horizontal_on_edge(void);
extern void test_near_edge_horizontal_within_tolerance(void);
extern void test_near_edge_horizontal_outside_tolerance(void);
extern void test_near_edge_horizontal_outside_range(void);
extern void test_near_edge_zero_tolerance(void);

int main(void) {
    UNITY_BEGIN();

    /* Edge creation & normalization */
    RUN_TEST(test_create_edge_normalizes_vertical_y_swap);
    RUN_TEST(test_create_edge_normalizes_horizontal_x_swap);
    RUN_TEST(test_create_edge_already_normalized);

    /* Edge equality */
    RUN_TEST(test_edge_equals_identical);
    RUN_TEST(test_edge_equals_reversed);
    RUN_TEST(test_edge_equals_null);
    RUN_TEST(test_edge_equals_different);

    /* Edge hash */
    RUN_TEST(test_edge_hash_same_for_equal_edges);

    /* Edge intersection */
    RUN_TEST(test_edge_intersects_vertical_overlapping);
    RUN_TEST(test_edge_intersects_horizontal_within);
    RUN_TEST(test_edge_intersects_horizontal_overlap);
    RUN_TEST(test_edge_intersects_same_corner);
    RUN_TEST(test_edge_intersects_non_intersecting);

    /* Rect */
    RUN_TEST(test_create_rect);
    RUN_TEST(test_rect_get_edges);

    /* EdgeList operations */
    RUN_TEST(test_edge_list_create);
    RUN_TEST(test_edge_list_add);
    RUN_TEST(test_edge_list_add_grows_capacity);
    RUN_TEST(test_edge_list_remove);
    RUN_TEST(test_edge_list_remove_nonexistent);
    RUN_TEST(test_edge_list_clear);

    /* Contour algorithm */
    RUN_TEST(test_contour_empty_input);
    RUN_TEST(test_contour_single_rectangle);
    RUN_TEST(test_contour_two_adjacent_rectangles);
    RUN_TEST(test_contour_l_shape_staggered);
    RUN_TEST(test_contour_t_junction);

    /* IsPointNearEdge */
    RUN_TEST(test_near_edge_vertical_on_edge);
    RUN_TEST(test_near_edge_vertical_within_tolerance);
    RUN_TEST(test_near_edge_vertical_outside_tolerance);
    RUN_TEST(test_near_edge_vertical_outside_range);
    RUN_TEST(test_near_edge_horizontal_on_edge);
    RUN_TEST(test_near_edge_horizontal_within_tolerance);
    RUN_TEST(test_near_edge_horizontal_outside_tolerance);
    RUN_TEST(test_near_edge_horizontal_outside_range);
    RUN_TEST(test_near_edge_zero_tolerance);

    return UNITY_END();
}
