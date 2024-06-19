namespace MultimonitorEdges
{
    public class Edge : IEquatable<Edge>
    {
        public int x1, x2, y1, y2;

        public Edge(int x1, int x2, int y1, int y2)
        {
            if (x1 == x2 && y1 > y2)
            {
                // Ensure y1 is always smaller
                int temp = y1;
                y1 = y2;
                y2 = temp;
            }
            else if (y1 == y2 && x1 > x2)
            {
                // Ensure x1 is always smaller
                int temp = x1;
                x1 = x2;
                x2 = temp;
            }

            this.x1 = x1;
            this.x2 = x2;
            this.y1 = y1;
            this.y2 = y2;
        }

        public bool Equals(Edge other)
        {
            if (other == null) return false;
            return (this.x1 == other.x1 && this.x2 == other.x2 && this.y1 == other.y1 && this.y2 == other.y2) ||
                   (this.x1 == other.x2 && this.x2 == other.x1 && this.y1 == other.y2 && this.y2 == other.y1);
        }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            return obj is Edge edge && Equals(edge);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(x1, x2, y1, y2);
        }

        public override string ToString()
        {
            return $"Edge({x1}, {x2}, {y1}, {y2})";
        }

        public bool Intersects(Edge other)
        {
            if (x1 == x2 && other.x1 == other.x2)
            {
                // Both edges are vertical
                return x1 == other.x1 && ((y1 >= other.y1 && y1 <= other.y2) || (y2 >= other.y1 && y2 <= other.y2));
            }
            else if (y1 == y2 && other.y1 == other.y2)
            {
                // Both edges are horizontal
                return y1 == other.y1 && ((x1 >= other.x1 && x1 <= other.x2) || (x2 >= other.x1 && x2 <= other.x2));
            }
            return false;
        }

    }

}
