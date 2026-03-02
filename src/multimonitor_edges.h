#ifndef MULTIMONITOR_EDGES_H
#define MULTIMONITOR_EDGES_H

#include <windows.h>
#include <stdbool.h>

typedef struct {
    int x1, x2, y1, y2;
} me_Edge;

typedef struct {
    int xMin, xMax, yMin, yMax;
} me_Rect;  // Changed from Rectangle to Rect to further avoid conflicts

// Edge functions
me_Edge me_create_edge(int x1, int x2, int y1, int y2);
bool me_edge_equals(const me_Edge* e1, const me_Edge* e2);
SIZE_T me_edge_hash(const me_Edge* e);
bool me_edge_intersects(const me_Edge* e1, const me_Edge* e2);

// Rectangle functions
me_Rect me_create_rect(int xMin, int xMax, int yMin, int yMax);
void me_rect_get_edges(const me_Rect* r, me_Edge edges[4]);

#endif // MULTIMONITOR_EDGES_H
