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

            // Determine if the edge is horizontal or vertical
            bool isHorizontal = edgeContainingPoint.y1 == edgeContainingPoint.y2;

            // Create a filtered list of potential opposite edges
            var filteredEdges = contourEdges
                // Opposite direction
                .Where(edge => edge.direction != edgeContainingPoint.direction)

                // Match orientation
                .Where(edge => (isHorizontal && edge.y1 == edge.y2)
                    || (!isHorizontal && edge.x1 == edge.x2))

                // Point in range
                .Where(edge => (isHorizontal && point.X >= edge.x1 && point.X <= edge.x2)
                    || (!isHorizontal && point.Y >= edge.y1 && point.Y <= edge.y2))

                .ToList();

            if (filteredEdges.Count == 0)
            {
                Console.WriteLine("No opposite edges found.");
                return null;
            }

            // Find the nearest opposite edge
            Edge nearestOppositeEdge = filteredEdges
                .OrderBy(edge => DistanceBetweenEdges(edgeContainingPoint, edge))
                .First();

            // Calculate the point on the nearest opposite edge
            return CalculatePointOnOppositeEdge(point, edgeContainingPoint, nearestOppositeEdge);
        }

        private static double DistanceBetweenEdges(Edge edge1, Edge edge2)
        {
            if (edge1.x1 == edge1.x2 && edge2.x1 == edge2.x2) // Both edges are vertical
            {
                return Math.Abs(edge1.x1 - edge2.x1);
            }
            else if (edge1.y1 == edge1.y2 && edge2.y1 == edge2.y2) // Both edges are horizontal
            {
                return Math.Abs(edge1.y1 - edge2.y1);
            }
            else
            {
                // This case should not occur if we've filtered correctly, but we'll include it for robustness
                throw new ArgumentException("Edges must be either both vertical or both horizontal");
            }
        }

        private static Point CalculatePointOnOppositeEdge(Point originalPoint, Edge originalEdge, Edge oppositeEdge)
        {
            if (oppositeEdge.x1 == oppositeEdge.x2) // Vertical edge
                return new Point(oppositeEdge.x1, originalPoint.Y);
            else
                return new Point(originalPoint.X, oppositeEdge.y1);
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

        private static Point GetEdgeDirectionVector(Edge edge)
        {
            Point direction = edge.x1 == edge.x2
                ? new Point(0, 1)  // Vertical edge
                : new Point(1, 0); // Horizontal edge

            return edge.direction ? direction : new Point(-direction.X, -direction.Y);
        }
    }
}