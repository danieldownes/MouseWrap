namespace MultimonitorEdges
{
    public class ContourSweepLine
    {
        public static List<Edge> GetContour(List<Rectangle> rectangles)
        {
            var events = new List<SweepLineEvent>();
            foreach (var rect in rectangles)
            {
                foreach (var edge in rect.GetEdges())
                {
                    events.Add(new SweepLineEvent(Math.Min(edge.x1, edge.x2), edge, true));
                    events.Add(new SweepLineEvent(Math.Max(edge.x1, edge.x2), edge, false));
                }
            }
            events.Sort();

            var activeEdges = new SortedSet<Edge>(new EdgeComparer());
            var contourEdges = new List<Edge>();

            foreach (var evt in events)
            {
                if (evt.IsStart)
                {
                    var overlappingEdges = activeEdges.Where(e => EdgesOverlap(e, evt.Edge)).ToList();
                    foreach (var overlappingEdge in overlappingEdges)
                    {
                        activeEdges.Remove(overlappingEdge);
                        contourEdges.AddRange(MergeEdges(overlappingEdge, evt.Edge));
                    }

                    if (overlappingEdges.Count == 0)
                    {
                        activeEdges.Add(evt.Edge);
                        contourEdges.Add(evt.Edge);
                    }
                }
                else
                {
                    activeEdges.Remove(evt.Edge);
                }
            }

            return MergeCollinearEdges(contourEdges);
        }

        private static bool EdgesOverlap(Edge e1, Edge e2)
        {
            return (e1.x1 == e1.x2 && e2.x1 == e2.x2 && e1.x1 == e2.x1 &&
                    Math.Max(e1.y1, e2.y1) <= Math.Min(e1.y2, e2.y2)) ||
                   (e1.y1 == e1.y2 && e2.y1 == e2.y2 && e1.y1 == e2.y1 &&
                    Math.Max(e1.x1, e2.x1) <= Math.Min(e1.x2, e2.x2));
        }

        private static List<Edge> MergeEdges(Edge e1, Edge e2)
        {
            var result = new List<Edge>();

            if (e1.x1 == e1.x2 && e2.x1 == e2.x2) // Vertical edges
            {
                int yMin = Math.Min(e1.y1, e2.y1);
                int yMax = Math.Max(e1.y2, e2.y2);
                result.Add(new Edge(e1.x1, e1.x2, yMin, yMax, e1.direction));
            }
            else if (e1.y1 == e1.y2 && e2.y1 == e2.y2) // Horizontal edges
            {
                int xMin = Math.Min(e1.x1, e2.x1);
                int xMax = Math.Max(e1.x2, e2.x2);
                result.Add(new Edge(xMin, xMax, e1.y1, e1.y2, e1.direction));
            }

            return result;
        }

        private static List<Edge> MergeCollinearEdges(List<Edge> edges)
        {
            var mergedEdges = new List<Edge>();
            var sortedEdges = edges.OrderBy(e => e.x1).ThenBy(e => e.y1).ToList();

            for (int i = 0; i < sortedEdges.Count; i++)
            {
                var currentEdge = sortedEdges[i];
                while (i + 1 < sortedEdges.Count && AreCollinear(currentEdge, sortedEdges[i + 1]))
                {
                    currentEdge = MergeCollinearEdge(currentEdge, sortedEdges[i + 1]);
                    i++;
                }
                mergedEdges.Add(currentEdge);
            }

            return mergedEdges;
        }

        private static bool AreCollinear(Edge e1, Edge e2)
        {
            return (e1.x1 == e1.x2 && e2.x1 == e2.x2 && e1.x1 == e2.x1) ||
                   (e1.y1 == e1.y2 && e2.y1 == e2.y2 && e1.y1 == e2.y1);
        }

        private static Edge MergeCollinearEdge(Edge e1, Edge e2)
        {
            if (e1.x1 == e1.x2 && e2.x1 == e2.x2) // Vertical edges
            {
                return new Edge(e1.x1, e1.x2, Math.Min(e1.y1, e2.y1), Math.Max(e1.y2, e2.y2), e1.direction);
            }
            else // Horizontal edges
            {
                return new Edge(Math.Min(e1.x1, e2.x1), Math.Max(e1.x2, e2.x2), e1.y1, e1.y2, e1.direction);
            }
        }

        private class EdgeComparer : IComparer<Edge>
        {
            public int Compare(Edge x, Edge y)
            {
                if (x.y1 != y.y1)
                    return x.y1.CompareTo(y.y1);
                return x.x1.CompareTo(y.x1);
            }
        }
    }
}