#include "multimonitor_contour.h"
#include <stdlib.h> // For qsort
#include <string.h> // For memmove (already used by MoveMemory, but good practice)

// Helper function to compare longs for qsort
static int compare_longs(const void* a, const void* b) {
    long la = *(const long*)a;
    long lb = *(const long*)b;
    if (la < lb) return -1;
    if (la > lb) return 1;
    return 0;
}

// Helper function to remove duplicates from a sorted array of longs and return the new size
static SIZE_T unique_sorted_longs(long* arr, SIZE_T size) {
    if (size == 0 || size == 1) return size;
    SIZE_T j = 0;
    for (SIZE_T i = 1; i < size; i++) {
        if (arr[i] != arr[j]) {
            j++;
            arr[j] = arr[i];
        }
    }
    return j + 1;
}

EdgeList* create_edge_list(SIZE_T initial_capacity) {
    EdgeList* list = (EdgeList*)HeapAlloc(GetProcessHeap(), 0, sizeof(EdgeList));
    list->edges = (me_Edge*)HeapAlloc(GetProcessHeap(), 0, initial_capacity * sizeof(me_Edge));
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

void edge_list_add(EdgeList* list, me_Edge edge) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->edges = (me_Edge*)HeapReAlloc(GetProcessHeap(), 0, list->edges, list->capacity * sizeof(me_Edge));
    }
    list->edges[list->size++] = edge;
}

void edge_list_remove(EdgeList* list, me_Edge edge) {
    for (SIZE_T i = 0; i < list->size; i++) {
        if (me_edge_equals(&list->edges[i], &edge)) {
            MoveMemory(&list->edges[i], &list->edges[i + 1], (list->size - i - 1) * sizeof(me_Edge));
            list->size--;
            return;
        }
    }
}

void edge_list_clear(EdgeList* list) {
    list->size = 0;
}

void edge_list_free(EdgeList* list) {
    HeapFree(GetProcessHeap(), 0, list->edges);
    HeapFree(GetProcessHeap(), 0, list);
}

// The remove_intersection function is no longer directly used by the new get_contour logic.
// It can be kept for other purposes or removed if not needed elsewhere.
// For now, I will comment it out.
/*
EdgeList* remove_intersection(me_Edge edge1, me_Edge edge2) {
    EdgeList* merged_edges = create_edge_list(2);

    // Check if the edges intersect horizontally
    if (edge1.y1 == edge1.y2 && edge2.y1 == edge2.y2 && edge1.y1 == edge2.y1) {
        // Find the intersection range of x-coordinates
        int merged_x_min = min(edge1.x1, edge2.x1);
        int merged_x_max = max(edge1.x2, edge2.x2);

        // Create two new edges representing the non-overlapping parts
        if (edge1.x1 < edge2.x1)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge2.x1, edge1.y1, edge1.y2)); // Left part
        if (edge1.x2 > edge2.x2)
            edge_list_add(merged_edges, me_create_edge(edge2.x2, edge1.x2, edge1.y1, edge1.y2)); // Right part
    }
    // Check if the edges intersect vertically
    else if (edge1.x1 == edge1.x2 && edge2.x1 == edge2.x2 && edge1.x1 == edge2.x1) {
        // Find the intersection range of y-coordinates
        int merged_y_min = min(edge1.y1, edge2.y1);
        int merged_y_max = max(edge1.y2, edge2.y2);

        // Create two new edges representing the non-overlapping parts
        if (edge1.y1 < edge2.y1)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge1.x2, edge1.y1, edge2.y1)); // Upper part
        if (edge1.y2 > edge2.y2)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge1.x2, edge2.y2, edge1.y2)); // Lower part
    }

    return merged_edges;
}
*/

EdgeList* get_contour(me_Rect* rectangles, SIZE_T rectangle_count) {
    if (rectangle_count == 0) {
        return create_edge_list(0);
    }

    // 1. Collect all initial full edges from rectangles
    EdgeList* initial_full_edges = create_edge_list(rectangle_count * 4);
    for (SIZE_T i = 0; i < rectangle_count; i++) {
        me_Edge rect_edges[4];
        me_rect_get_edges(&rectangles[i], rect_edges); // Assumes this gives {L, R, T, B}
        for (int j = 0; j < 4; j++) {
            edge_list_add(initial_full_edges, rect_edges[j]);
        }
    }

    // 2. Collect all unique X and Y coordinates from the initial edges
    // Max 2 distinct x-coords and 2 distinct y-coords per edge.
    long* x_coords_buffer = (long*)HeapAlloc(GetProcessHeap(), 0, initial_full_edges->size * 2 * sizeof(long));
    long* y_coords_buffer = (long*)HeapAlloc(GetProcessHeap(), 0, initial_full_edges->size * 2 * sizeof(long));
    if (!x_coords_buffer || !y_coords_buffer) {
        edge_list_free(initial_full_edges);
        if (x_coords_buffer) HeapFree(GetProcessHeap(), 0, x_coords_buffer);
        if (y_coords_buffer) HeapFree(GetProcessHeap(), 0, y_coords_buffer);
        return create_edge_list(0); // Allocation failure
    }
    SIZE_T x_coord_count = 0;
    SIZE_T y_coord_count = 0;

    for (SIZE_T i = 0; i < initial_full_edges->size; i++) {
        me_Edge e = initial_full_edges->edges[i];
        // For vertical edges, their x1 (and x2) is an x-coordinate. Their y1 and y2 are y-coordinates.
        // For horizontal edges, their y1 (and y2) is a y-coordinate. Their x1 and x2 are x-coordinates.
        x_coords_buffer[x_coord_count++] = e.x1;
        x_coords_buffer[x_coord_count++] = e.x2;
        y_coords_buffer[y_coord_count++] = e.y1;
        y_coords_buffer[y_coord_count++] = e.y2;
    }

    qsort(x_coords_buffer, x_coord_count, sizeof(long), compare_longs);
    qsort(y_coords_buffer, y_coord_count, sizeof(long), compare_longs);

    x_coord_count = unique_sorted_longs(x_coords_buffer, x_coord_count);
    y_coord_count = unique_sorted_longs(y_coords_buffer, y_coord_count);

    // 3. Decompose initial edges into minimal segments based on the unique coordinates
    EdgeList* minimal_segments = create_edge_list(initial_full_edges->size * (x_coord_count + y_coord_count)); // Rough estimate

    for (SIZE_T i = 0; i < initial_full_edges->size; i++) {
        me_Edge original_edge = initial_full_edges->edges[i];
        if (original_edge.x1 == original_edge.x2) { // Vertical edge at x = original_edge.x1
            long x = original_edge.x1;
            for (SIZE_T k = 0; k < y_coord_count; k++) {
                if (k + 1 >= y_coord_count) break; // Need a pair of y-coords to form a segment
                long y_seg_start = y_coords_buffer[k];
                long y_seg_end = y_coords_buffer[k + 1];

                // Check if this minimal y-span [y_seg_start, y_seg_end] is part of the original_edge
                if (y_seg_start < y_seg_end && // Valid segment
                    y_seg_start >= original_edge.y1 && y_seg_end <= original_edge.y2) {
                    edge_list_add(minimal_segments, me_create_edge(x, x, y_seg_start, y_seg_end));
                }
            }
        } else { // Horizontal edge at y = original_edge.y1
            long y = original_edge.y1;
            for (SIZE_T k = 0; k < x_coord_count; k++) {
                if (k + 1 >= x_coord_count) break;
                long x_seg_start = x_coords_buffer[k];
                long x_seg_end = x_coords_buffer[k + 1];

                if (x_seg_start < x_seg_end && // Valid segment
                    x_seg_start >= original_edge.x1 && x_seg_end <= original_edge.x2) {
                    edge_list_add(minimal_segments, me_create_edge(x_seg_start, x_seg_end, y, y));
                }
            }
        }
    }

    HeapFree(GetProcessHeap(), 0, x_coords_buffer);
    HeapFree(GetProcessHeap(), 0, y_coords_buffer);
    edge_list_free(initial_full_edges);

    // 4. Apply pairing logic to the list of minimal_segments
    EdgeList* contour_edges = create_edge_list(minimal_segments->size);
    BOOL* removed = (BOOL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, minimal_segments->size * sizeof(BOOL));
    if (removed == NULL && minimal_segments->size > 0) {
        edge_list_free(minimal_segments);
        return create_edge_list(0); // Allocation failure
    }

    for (SIZE_T i = 0; i < minimal_segments->size; i++) {
        if (removed[i]) {
            continue;
        }
        BOOL pair_found = FALSE;
        for (SIZE_T j = i + 1; j < minimal_segments->size; j++) {
            if (removed[j]) {
                continue;
            }
            if (me_edge_equals(&minimal_segments->edges[i], &minimal_segments->edges[j])) {
                removed[i] = TRUE;
                removed[j] = TRUE;
                pair_found = TRUE;
                break;
            }
        }
        if (!pair_found) {
            edge_list_add(contour_edges, minimal_segments->edges[i]);
        }
    }

    HeapFree(GetProcessHeap(), 0, removed);
    edge_list_free(minimal_segments);

    // 5. Optional: Merge adjacent collinear segments in contour_edges for cleaner output.
    //    For now, returning unmerged segments. If the logic is correct, the wrap behavior
    //    should be fine even with segmented contour edges.

    return contour_edges;
}