#include "multimonitor_edges.h"
#include <string.h>

me_Edge me_create_edge(int x1, int x2, int y1, int y2) {
    me_Edge e;
    if (x1 == x2 && y1 > y2) {
        // Ensure y1 is always smaller
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    else if (y1 == y2 && x1 > x2) {
        // Ensure x1 is always smaller
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    e.x1 = x1;
    e.x2 = x2;
    e.y1 = y1;
    e.y2 = y2;
    return e;
}

bool me_edge_equals(const me_Edge* e1, const me_Edge* e2) {
    if (e1 == NULL || e2 == NULL) return false;
    return (e1->x1 == e2->x1 && e1->x2 == e2->x2 && e1->y1 == e2->y1 && e1->y2 == e2->y2) ||
        (e1->x1 == e2->x2 && e1->x2 == e2->x1 && e1->y1 == e2->y2 && e1->y2 == e2->y1);
}

size_t me_edge_hash(const me_Edge* e) {
    // A simple hash function, you might want to use a more sophisticated one
    return (size_t)(e->x1 ^ e->x2 ^ e->y1 ^ e->y2);
}

void me_edge_to_string(const me_Edge* e, char* buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "Edge(%d, %d, %d, %d)", e->x1, e->x2, e->y1, e->y2);
}

bool me_edge_intersects(const me_Edge* e1, const me_Edge* e2) {
    // Check if both edges are horizontal and overlapping on the same y-coordinate
    if (e1->y1 == e1->y2 && e2->y1 == e2->y2 && e1->y1 == e2->y1) {
        // Check if the x-coordinate ranges of the two edges overlap
        return (e1->x1 <= e2->x2 && e1->x2 >= e2->x1) || (e2->x1 <= e1->x2 && e2->x2 >= e1->x1);
    }
    // Check if both edges are vertical and overlapping on the same x-coordinate
    if (e1->x1 == e1->x2 && e2->x1 == e2->x2 && e1->x1 == e2->x1) {
        // Check if the y-coordinate ranges of the two edges overlap
        return (e1->y1 <= e2->y2 && e1->y2 >= e2->y1) || (e2->y1 <= e1->y2 && e2->y2 >= e1->y1);
    }
    return false;
}

me_Rect me_create_rect(int xMin, int xMax, int yMin, int yMax) {
    me_Rect r = { xMin, xMax, yMin, yMax };
    return r;
}

void me_rect_get_edges(const me_Rect* r, me_Edge edges[4]) {
    edges[0] = me_create_edge(r->xMin, r->xMin, r->yMin, r->yMax); // Left edge
    edges[1] = me_create_edge(r->xMax, r->xMax, r->yMin, r->yMax); // Right edge
    edges[2] = me_create_edge(r->xMin, r->xMax, r->yMin, r->yMin); // Bottom edge
    edges[3] = me_create_edge(r->xMin, r->xMax, r->yMax, r->yMax); // Top edge
}