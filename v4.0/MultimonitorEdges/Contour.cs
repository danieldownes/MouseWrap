namespace MultimonitorEdges;

public class Contour
{
    public static List<Edge> GetContour(List<Rectangle> rectangles)
    {
        var edges = new List<Edge>();

        foreach (var rect in rectangles)
        {
            var newEdges = new List<Edge>();

            foreach (var edge in rect.GetEdges())
            {
                bool isIntersecting = false;

                foreach (var existingEdge in edges.ToList()) // Iterate over a copy to avoid modification during iteration
                {
                    if (edge.Intersects(existingEdge))
                    {
                        isIntersecting = true;
                        newEdges.AddRange(RemoveIntersection(edge, existingEdge));
                        edges.Remove(existingEdge);
                        continue;
                    }
                }

                if (!isIntersecting)
                    newEdges.Add(edge);
            }

            edges.AddRange(newEdges);
        }

        return edges;
    }



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
                mergedEdges.Add(new Edge(edge1.x1, edge2.x1, edge1.y1, edge1.y2)); // Left part
            if (edge1.x2 > edge2.x2)
                mergedEdges.Add(new Edge(edge2.x2, edge1.x2, edge1.y1, edge1.y2)); // Right part

        }
        // Check if the edges intersect vertically
        else if (edge1.x1 == edge1.x2 && edge2.x1 == edge2.x2 && edge1.x1 == edge2.x1)
        {
            // Find the intersection range of y-coordinates
            int mergedYMin = Math.Min(edge1.y1, edge2.y1);
            int mergedYMax = Math.Max(edge1.y2, edge2.y2);

            // Create two new edges representing the non-overlapping parts
            if (edge1.y1 < edge2.y1)
                mergedEdges.Add(new Edge(edge1.x1, edge1.x2, edge1.y1, edge2.y1)); // Upper part
            if (edge1.y2 > edge2.y2)
                mergedEdges.Add(new Edge(edge1.x1, edge1.x2, edge2.y2, edge1.y2)); // Lower part

        }

        return mergedEdges;
    }

}
