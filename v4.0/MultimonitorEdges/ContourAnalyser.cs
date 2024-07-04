namespace MultimonitorEdges
{
    public struct Point
    {
        public int X { get; }
        public int Y { get; }

        public Point(int x, int y)
        {
            X = x;
            Y = y;
        }
    }

    public class ContourAnalyser
    {
        public static Point? GetOppositePointInContour(Point point, List<Edge> contourEdges)
        {
            // Validate that the point is on an edge
            Edge edgeContainingPoint = contourEdges.FirstOrDefault(edge => IsPointOnEdge(point, edge));

            if (edgeContainingPoint == null)
            {
                Console.WriteLine("The provided point is not on any edge of the contour.");
                return null;
            }

            // Determine the direction vector of the edge
            Point direction = GetEdgeDirection(edgeContainingPoint);

            // Start from the given point and move in the direction of the edge
            Point currentPoint = point;
            Point startPoint = point;
            while (true)
            {
                currentPoint = new Point(currentPoint.X + direction.X, currentPoint.Y + direction.Y);

                // Check if we've reached the end of the current edge
                if (!IsPointOnEdge(currentPoint, edgeContainingPoint))
                {
                    // Find the next edge
                    Edge nextEdge = FindNextEdge(currentPoint, contourEdges);
                    if (nextEdge == null)
                    {
                        // We've reached a corner, return the last point on the current edge
                        return new Point(currentPoint.X - direction.X, currentPoint.Y - direction.Y);
                    }

                    // Update the direction for the new edge
                    direction = GetEdgeDirection(nextEdge);
                    edgeContainingPoint = nextEdge;
                }

                // Check if we've completed a full loop
                if (currentPoint.X == startPoint.X && currentPoint.Y == startPoint.Y)
                {
                    Console.WriteLine("Completed a full loop without finding an opposite point.");
                    return null;
                }
            }
        }

        private static bool IsPointOnEdge(Point point, Edge edge)
        {
            if (edge.x1 == edge.x2) // Vertical edge
            {
                return point.X == edge.x1 &&
                       point.Y >= Math.Min(edge.y1, edge.y2) &&
                       point.Y <= Math.Max(edge.y1, edge.y2);
            }
            else // Horizontal edge
            {
                return point.Y == edge.y1 &&
                       point.X >= Math.Min(edge.x1, edge.x2) &&
                       point.X <= Math.Max(edge.x1, edge.x2);
            }
        }

        private static Edge FindNextEdge(Point point, List<Edge> edges)
        {
            return edges.FirstOrDefault(edge =>
                (edge.x1 == point.X && edge.y1 == point.Y) ||
                (edge.x2 == point.X && edge.y2 == point.Y));
        }

        private static Point GetEdgeDirection(Edge edge)
        {
            Point direction = edge.x1 == edge.x2
                ? new Point(0, 1)  // Vertical edge
                : new Point(1, 0); // Horizontal edge

            return edge.direction ? direction : new Point(-direction.X, -direction.Y);
        }
    }
}