#include "multimonitor_contour.h"
#include <stdlib.h>
#include <string.h>

EdgeList* create_edge_list(size_t initial_capacity) {
    EdgeList* list = (EdgeList*)malloc(sizeof(EdgeList));
    list->edges = (me_Edge*)malloc(initial_capacity * sizeof(me_Edge));
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

void edge_list_add(EdgeList* list, me_Edge edge) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->edges = (me_Edge*)realloc(list->edges, list->capacity * sizeof(me_Edge));
    }
    list->edges[list->size++] = edge;
}

void edge_list_remove(EdgeList* list, me_Edge edge) {
    for (size_t i = 0; i < list->size; i++) {
        if (me_edge_equals(&list->edges[i], &edge)) {
            memmove(&list->edges[i], &list->edges[i + 1], (list->size - i - 1) * sizeof(me_Edge));
            list->size--;
            return;
        }
    }
}

void edge_list_clear(EdgeList* list) {
    list->size = 0;
}

void edge_list_free(EdgeList* list) {
    free(list->edges);
    free(list);
}

EdgeList* remove_intersection(me_Edge edge1, me_Edge edge2) {
    EdgeList* merged_edges = create_edge_list(2);

    // Check if the edges intersect horizontally
    if (edge1.y1 == edge1.y2 && edge2.y1 == edge2.y2 && edge1.y1 == edge2.y1) {
        // Find the intersection range of x-coordinates
        int merged_x_min = (edge1.x1 < edge2.x1) ? edge1.x1 : edge2.x1;
        int merged_x_max = (edge1.x2 > edge2.x2) ? edge1.x2 : edge2.x2;

        // Create two new edges representing the non-overlapping parts
        if (edge1.x1 < edge2.x1)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge2.x1, edge1.y1, edge1.y2)); // Left part
        if (edge1.x2 > edge2.x2)
            edge_list_add(merged_edges, me_create_edge(edge2.x2, edge1.x2, edge1.y1, edge1.y2)); // Right part
    }
    // Check if the edges intersect vertically
    else if (edge1.x1 == edge1.x2 && edge2.x1 == edge2.x2 && edge1.x1 == edge2.x1) {
        // Find the intersection range of y-coordinates
        int merged_y_min = (edge1.y1 < edge2.y1) ? edge1.y1 : edge2.y1;
        int merged_y_max = (edge1.y2 > edge2.y2) ? edge1.y2 : edge2.y2;

        // Create two new edges representing the non-overlapping parts
        if (edge1.y1 < edge2.y1)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge1.x2, edge1.y1, edge2.y1)); // Upper part
        if (edge1.y2 > edge2.y2)
            edge_list_add(merged_edges, me_create_edge(edge1.x1, edge1.x2, edge2.y2, edge1.y2)); // Lower part
    }

    return merged_edges;
}

EdgeList* get_contour(me_Rect* rectangles, size_t rectangle_count) {
    EdgeList* edges = create_edge_list(rectangle_count * 4);

    for (size_t i = 0; i < rectangle_count; i++) {
        me_Edge rect_edges[4];
        me_rect_get_edges(&rectangles[i], rect_edges);

        EdgeList* new_edges = create_edge_list(4);

        for (int j = 0; j < 4; j++) {
            bool is_intersecting = false;

            for (size_t k = 0; k < edges->size; k++) {
                if (me_edge_intersects(&rect_edges[j], &edges->edges[k])) {
                    is_intersecting = true;
                    EdgeList* intersection_removed = remove_intersection(rect_edges[j], edges->edges[k]);

                    for (size_t l = 0; l < intersection_removed->size; l++) {
                        edge_list_add(new_edges, intersection_removed->edges[l]);
                    }

                    edge_list_remove(edges, edges->edges[k]);
                    edge_list_free(intersection_removed);
                    break;
                }
            }

            if (!is_intersecting) {
                edge_list_add(new_edges, rect_edges[j]);
            }
        }

        for (size_t j = 0; j < new_edges->size; j++) {
            edge_list_add(edges, new_edges->edges[j]);
        }

        edge_list_free(new_edges);
    }

    return edges;
}