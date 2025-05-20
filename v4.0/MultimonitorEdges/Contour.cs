namespace MultimonitorEdges;

public class Contour
{
    public static List<Edge> GetContour(List<Rectangle> rectangles)
    {
        var allEdges = new List<Edge>();
        foreach (var rect in rectangles)
        {
            allEdges.AddRange(rect.GetEdges());
        }

        var contourEdges = new List<Edge>();
        // Keep track of indices of edges that have been paired and removed.
        var removed = new bool[allEdges.Count];

        for (int i = 0; i < allEdges.Count; i++)
        {
            if (removed[i])
            {
                continue; // This edge was already part of a removed pair
            }

            bool pairFound = false;
            for (int j = i + 1; j < allEdges.Count; j++)
            {
                if (removed[j])
                {
                    continue; // This edge was already part of a removed pair
                }

                // Edge.Equals checks for geometric equality, ignoring direction.
                // The constructor normalizes edges, so x1<=x2 and y1<=y2.
                if (allEdges[i].Equals(allEdges[j]))
                {
                    // Found an identical edge. This pair represents an internal shared boundary.
                    removed[i] = true;
                    removed[j] = true;
                    pairFound = true;
                    break; // Found a pair for allEdges[i], no need to check further for it
                }
            }

            if (!pairFound)
            {
                // This edge has no identical counterpart, so it's an outer edge.
                contourEdges.Add(allEdges[i]);
            }
        }
        return contourEdges;
    }

    // The RemoveIntersection method is no longer used by the simplified GetContour.
    // It can be kept for other purposes or removed if not needed elsewhere.
    // For now, I will comment it out to indicate it's not part of the active contour logic.
    /*
    private static List<Edge> RemoveIntersection(Edge edge1, Edge edge2)
    {
        List<Edge> mergedEdges = new List<Edge>();

        // Check if the edges intersect horizontally
        if (edge1.y1 == edge1.y2 && edge2.y1 == edge2.y2 && edge1.y1 == edge2.y1)
        {
            // Find the intersection range of x-coordinates
            int mergedXMin = Math.Min(edge1.x1, edge2.x1);
            int mergedXMax = Math.Max(edge1.x2, edge2.x2);

            // Create two new edges representing the non-overlapping parts
            if (edge1.x1 < edge2.x1)
                mergedEdges.Add(new Edge(edge1.x1, edge2.x1, edge1.y1, edge1.y2, true)); // Left part
            if (edge1.x2 > edge2.x2)
                mergedEdges.Add(new Edge(edge2.x2, edge1.x2, edge1.y1, edge1.y2, false)); // Right part

        }
        // Check if the edges intersect vertically
        else if (edge1.x1 == edge1.x2 && edge2.x1 == edge2.x2 && edge1.x1 == edge2.x1)
        {
            // Find the intersection range of y-coordinates
            int mergedYMin = Math.Min(edge1.y1, edge2.y1);
            int mergedYMax = Math.Max(edge1.y2, edge2.y2);

            // Create two new edges representing the non-overlapping parts
            if (edge1.y1 < edge2.y1)
                mergedEdges.Add(new Edge(edge1.x1, edge1.x2, edge1.y1, edge2.y1, false)); // Upper part
            if (edge1.y2 > edge2.y2)
                mergedEdges.Add(new Edge(edge1.x1, edge1.x2, edge2.y2, edge1.y2, true)); // Lower part

        }

        return mergedEdges;
    }
    */
}
