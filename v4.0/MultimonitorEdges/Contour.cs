namespace MultimonitorEdges
{
    internal class Contour
    {
        public static List<Edge> GetContour(List<Rectangle> rectangles)
        {
            var edges = new List<Edge>();

            foreach (var rect in rectangles)
            {
                foreach (var edge in rect.GetEdges())
                {
                    bool isIntersecting = false;
                    foreach (var existingEdge in edges.ToList())
                    {
                        if (edge.Intersects(existingEdge))
                        {
                            isIntersecting = true;
                            edges.Remove(existingEdge);
                            edges.Add(new Edge(Math.Min(edge.x1, existingEdge.x1), Math.Max(edge.x2, existingEdge.x2),
                                               Math.Min(edge.y1, existingEdge.y1), Math.Max(edge.y2, existingEdge.y2)));
                            break;
                        }
                    }
                    if (!isIntersecting)
                        edges.Add(edge);
                }
            }

            return edges;
        }
    }
}
