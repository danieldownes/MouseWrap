﻿namespace MultimonitorEdges
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
            // Check if both edges are horizontal and overlapping on the same y-coordinate
            if (y1 == y2 && other.y1 == other.y2 && y1 == other.y1)
            {
                // Check if the x-coordinate ranges of the two edges overlap
                return (x1 <= other.x2 && x2 >= other.x1) || (other.x1 <= x2 && other.x2 >= x1);
            }

            // Check if both edges are vertical and overlapping on the same x-coordinate
            if (x1 == x2 && other.x1 == other.x2 && x1 == other.x1)
            {
                // Check if the y-coordinate ranges of the two edges overlap
                return (y1 <= other.y2 && y2 >= other.y1) || (other.y1 <= y2 && other.y2 >= y1);
            }

            return false;
        }

    }

}
