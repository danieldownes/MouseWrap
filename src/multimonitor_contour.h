#ifndef MULTIMONITOR_CONTOUR_H
#define MULTIMONITOR_CONTOUR_H

#include <windows.h>
#include "multimonitor_edges.h"

typedef struct {
    me_Edge* edges;
    size_t size;
    size_t capacity;
} EdgeList;

// EdgeList functions
EdgeList* create_edge_list(size_t initial_capacity);
void edge_list_add(EdgeList* list, me_Edge edge);
void edge_list_remove(EdgeList* list, me_Edge edge);
void edge_list_clear(EdgeList* list);
void edge_list_free(EdgeList* list);

// Contour functions
EdgeList* get_contour(me_Rect* rectangles, size_t rectangle_count);
EdgeList* remove_intersection(me_Edge edge1, me_Edge edge2);

#endif // MULTIMONITOR_CONTOUR_H
